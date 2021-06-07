import requests
import datetime
import json
import pandas as pd
import sqlite3 as lite
from sqlite3 import Error
from flask import Flask, jsonify
from flask import request
from flask_restplus import Resource, Api

app = Flask(__name__)
api = Api(app)


def collection_id(c,indicator_id):
	table_name = 'collection_id'
	query_insert = "INSERT INTO collection_id (creation_time,indicator) VALUES (?,?);"
	query_s = "SELECT id FROM collection_id WHERE indicator=?;"
	if table_exists(c,table_name):
		time = datetime.datetime.utcnow()
		c.execute(query_insert,(time,indicator_id,))
		conn.commit()
	else:
		c.execute('''CREATE TABLE collection_id
             (id INTEGER PRIMARY KEY AUTOINCREMENT,
              creation_time timestamp,
              indicator varchar(100)
              )''')
		conn.commit()
		time = datetime.datetime.utcnow()
		c.execute(query_insert,(time,indicator_id,))
		conn.commit()
	c.execute(query_s,(indicator_id,))
	conn.commit()
	id_num = c.fetchone()[0]
	return id_num 

def return_json(url):
	valid = True
	headers = {'Accept':'application/json'}
	response = requests.get(url,headers=headers)
	data = response.json()
	if len(data)<=1 and data[0]['message'][0]['key'] == 'Invalid value':
		valid = False
		data = {
		"message":"Please input a valid indicator value"
		}
	return valid, data

def table_exists(c,table_name):
	query = "SELECT count(name) FROM sqlite_master WHERE type='table' AND name=?;"
	c.execute(query,(table_name,))
	conn.commit()
	if c.fetchone()[0] == 1:
		return True
	return False

@api.route('/collections')
class post_collections(Resource):
	@api.doc(params={'indicator_id': 'An ID'})
	@api.response(200,'This collection has already been imported')
	@api.response(201,'Successfully imported a new collection')
	@api.response(404,'Bad attempt')
	def post(self):
		indicator_id = request.args.get('indicator_id')
		indicator_id = indicator_id.strip()
		url = 'http://api.worldbank.org/v2/countries/all/indicators/'+indicator_id+'?date=2012:2017&format=json&per_page=1000'
		valid,data = return_json(url)
		if valid:
			indicator_id = data[1][0]['indicator']['id']
			df = pd.read_json(json.dumps(data[1]))
			df['indicator'] = df['indicator'].apply(lambda x: x['value'])
			df['country'] = df['country'].apply(lambda x: x['value'])
			df = df.dropna(subset=['value'])
			table_name = indicator_id
			c = conn.cursor()
			if table_exists(c,table_name):
				query_s = "SELECT id FROM collection_id WHERE indicator=?;"
				c.execute(query_s,(indicator_id,))
				conn.commit()
				id_num = c.fetchone()[0]
				return {
					"id": id_num,
					"message":"This collection has already been imported"
				},200
			else:
				id_num = collection_id(c,indicator_id)
				df.to_sql(name=table_name,con=conn)
				return {
					"uri": f"/collections/{id_num}",
					"id": id_num,
					"creation_time": str(datetime.datetime.utcnow()),
					"indicator_id": indicator_id
				},201
		else:
			return {
				"message":"Indicator id is not valid"
			},404

	@api.doc(params={'order_by': 'choose to sort the collection on "id, creation_time, indicator","+" ascending; "-" descending'},type='array')
	@api.response(200,'Successfully retrieve the list of available collections')
	@api.response(404,'Bad attempt')
	def get(self):
		c = conn.cursor()
		table_name = 'collection_id'
		sort_l = []
		order_s = request.args.get('order_by')
		if table_exists(c,table_name):
			sort_s = 'SELECT id,creation_time,indicator FROM collection_id ORDER BY {}'
			if order_s:
				order_list = order_s.split(',')
				for e in order_list:
					e = e.strip()
					if not e:
						sort_s = 'SELECT id,creation_time,indicator FROM collection_id'
						c.execute(sort_s.format((',').join(sort_l)))
						conn.commit()
					else:
						if e[0] == '+':
							s = e[1:] + ' ASC'
							sort_l.append(s)
						elif e[0] == '-':
							s = e[1:] + ' DESC'
							sort_l.append(s)
						else:
							return {
								"message":"Please add '+' or '-' before to decide your sorting order"
							},404
						if e[1:] not in ['id','creation_time','indicator']:
							return {
								"message":"Please input a valid query"
							},404
						sort_s = 'SELECT id,creation_time,indicator FROM collection_id ORDER BY {}'
						c.execute(sort_s.format((',').join(sort_l)))
						conn.commit()
			else:
				sort_s = 'SELECT id,creation_time,indicator FROM collection_id'
				c.execute(sort_s)
				conn.commit()
			a = c.fetchall()
			message_l = []
			for e in a:
				message = {
					"uri":f"/collections/{e[0]}",
					"id": e[0],
					"creation_time": e[1],
					"indicator": e[2]
				}
				message_l.append(message)
			return message_l,200
		else:
			return {"message":"There is not any collections in database"}, 404


@api.route('/collections/<int:id>')
class delete_collection(Resource):
	@api.response(200,'Successfully remove this collection')
	@api.response(404,'Bad attempt')
	def delete(self,id):
		id_num = id
		query_s = "SELECT indicator FROM collection_id WHERE id=?;"
		delete_s = "DELETE FROM collection_id WHERE id=?;"
		c = conn.cursor()
		if table_exists(c,'collection_id'):
			c.execute(query_s,(id_num,))
			conn.commit()
			a = c.fetchone()
			if a != None:
				table_name = a[0]
				drop_table = f"DROP TABLE '{table_name}'"
				c.execute(drop_table)
				conn.commit()
				c.execute(delete_s,(id_num,))
				conn.commit()
				return {
					"message" :f"The collection {id_num} was successfully removed from the database!",
    				"id": id_num
				},200
			return {
				"message": f"id {id_num} does not exist "
			},404
		return {
			"message":"There is not any collections in database"
		},404

	@api.response(200,'Successfully retrieve a collection')
	@api.response(404,'Bad attempt')
	def get(self,id):
		id_num = id
		query_s = "SELECT indicator,creation_time FROM collection_id WHERE id=?;"
		c = conn.cursor()
		if table_exists(c,'collection_id'):
			c.execute(query_s,(id_num,))
			conn.commit()
			a = c.fetchone()
			if a != None:
				table_name = a[0]
				creation_time = a[1]
				indicator_id = table_name
				query_df = f"SELECT indicator,country,date,value FROM '{table_name}'"
				df_clean = pd.read_sql_query(query_df,conn)
				q = df_clean.apply(lambda x: {"country":x['country'],"date":x['date'],"value":x['value']},axis=1)
				q = q.tolist()				
				return {
					"id": id_num,
					"indicator": indicator_id,
					"indicator_value": df_clean.indicator[0],
					"creation_time": creation_time,
					"entries": q
				},200
			else:
				return {
					"message": f"id {id_num} does not exist "
				},404
		else:
			return {
				"message":"There is not any collections in database"
			},404


@api.route('/collections/<int:id>/<year>/<string:country>')
class retrieve_indicator(Resource):
	@api.response(200,'Successfully retrieve economic indicator value for given country and a year')
	@api.response(404,'Bad attempt')
	def get(self,id,year,country):
		id_num = id
		query_s = 'SELECT indicator FROM collection_id WHERE id=?;'
		c = conn.cursor()
		if table_exists(c,'collection_id'):
			c.execute(query_s,(id_num,))
			conn.commit()
			a = c.fetchone()
			if a != None:
				indicator = a[0]
				query_a = f"SELECT value FROM '{indicator}' WHERE date=? AND country=?;"
				c.execute(query_a,(year,country,))
				conn.commit()
				b = c.fetchone()
				if b != None:
					value = b[0]
					return {
						"id": id_num,
						"indicator": indicator,
						"country": country,
						"year": year,
						"value": value
					},200
				else: 
					return {
						"message": "year or country does not exist"
					}, 404
			else:
				return {
					"message": f"id {id_num} does not exist "
				}, 404
		else:
			return {
				"message":"There is not any collections in database"
			}, 404

@api.route('/collections/<int:id>/<year>')
class retrieve_n_indicator(Resource):
	@api.doc(params={'q': 'your query method and it should be +N or -N'})
	@api.response(200,'Successfully retrieve top/bottom economic indicator values for a given year')
	@api.response(404,'Bad attempt')
	def get(self,id,year):
		q = request.args.get('q')
		id_num = id
		query_s = 'SELECT indicator FROM collection_id WHERE id=?;'
		c = conn.cursor()
		if table_exists(c,'collection_id'):
			c.execute(query_s,(id_num,))
			conn.commit()
			a = c.fetchone()
			if a:
				indicator = a[0]
				if q:
					if q[0] == '+' or q[0] == '-':
						try:
							N = int(q[1:])
							if N < 1 or N > 100:
								return {
									"message": "N should between 1 and 100"
								},404
						except ValueError as e:
							return {
								"message":"Please input an integer"
							},404
						n_dict = {'+':f'SELECT indicator,country,value FROM "{indicator}" WHERE date=? ORDER BY value DESC LIMIT "{N}";',
							  	  '-':f'SELECT * FROM (SELECT indicator,country,value FROM "{indicator}" WHERE date=? ORDER BY value LIMIT "{N}") ORDER BY value DESC;'}
						query_a = n_dict[q[0]]
					else:
						return {
							"message":"Please input correct format '+N' or '-N'"
						},404
				else:
					query_a = f"SELECT indicator,country,value FROM '{indicator}' WHERE date=?;"

				c.execute(query_a,(year,))
				conn.commit()
				b = c.fetchall()
				if b:
					indicator_value = b[0][0]
					entries = []
					for e in b:
						entries.append({
							"country":e[1],
							"value":e[2]
							})
					return {
						"indicator": indicator,
						"indicator_value": indicator_value,
						"entries": entries
					}
				else:
					return {
						"message": f"The year {year} does not exist"
					},404
			else:
				return {
					"message":f"id {id_num} does not exist "
				},404
		else:
			return {
				"message":"There is not any collections in database"
			}, 404


if __name__ == '__main__':
	conn_file = 'z5199285.db'
	conn = lite.connect(conn_file,check_same_thread=False)
	app.run(debug=True)
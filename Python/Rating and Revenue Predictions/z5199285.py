import sys
import json
import pandas as pd
import numpy as np
from sklearn import tree
from sklearn import svm
from sklearn.svm import SVC
from datetime import datetime
from sklearn import linear_model
from sklearn.utils import shuffle
from sklearn.naive_bayes import GaussianNB
from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neighbors import KNeighborsRegressor
from sklearn.model_selection import cross_val_score
from sklearn.linear_model import LogisticRegression
from sklearn.preprocessing import PolynomialFeatures
from sklearn.model_selection import RandomizedSearchCV
from sklearn.naive_bayes import BernoulliNB,MultinomialNB
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.linear_model import LinearRegression,BayesianRidge
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.ensemble import RandomForestRegressor, RandomForestClassifier
from sklearn.model_selection import GridSearchCV,train_test_split,ShuffleSplit
from sklearn.metrics import mean_squared_error,precision_score,recall_score,accuracy_score,confusion_matrix


def encode_column(df_train,df_valid,e,max_features=100):
    count = CountVectorizer(max_features = max_features)
    X = count.fit(df_train[e])
    X_train = X.transform(df_train[e])
    X_test = X.transform(df_valid[e])
    X_train = pd.DataFrame(X_train.todense(),columns=count.get_feature_names())
    X_test = pd.DataFrame(X_test.todense(),columns=count.get_feature_names())
    return X_train,X_test

def is_json(myjson):
    try:
        json_object = json.loads(myjson)
    except:
        return False
    return True

def return_str(x,json_id):
    return ','.join([item[f'{json_id}'].replace(" ","") for item in json.loads(x)])


def preprocess(df_train,col_l,drop_cols):
    df_train = df_train.drop(drop_cols,axis=1)
    for col in col_l:
        if col == 'spoken_languages':
            df_train['spoken_languages'] = df_train['spoken_languages'].apply(lambda x: return_str(x,'iso_639_1'))
        elif col == 'release_date':
            #df_train['release_year'] = df_train['release_date'].apply(lambda x: int(x.split('-')[0]))
            df_train['release_date'] = df_train['release_date'].apply(lambda x: datetime.strptime(x, '%Y-%m-%d').timetuple().tm_yday)
            
        else:
            df_train[col] = df_train[col].apply(lambda x: return_str(x,'name'))
    return df_train


def get_encode_df(df_train,df_valid,col_l,drop_cols,col_names):
    df_train = preprocess(df_train,col_l,drop_cols)
    df_valid = preprocess(df_valid,col_l,drop_cols)
    for e in col_names.keys():
        df_encode_train,df_encode_valid = encode_column(df_train,df_valid,e,max_features=col_names[e])
        df_train = pd.concat([df_train, df_encode_train],axis=1)
        df_valid = pd.concat([df_valid, df_encode_valid],axis=1)
        df_train = df_train.drop(e,axis=1)
        df_valid = df_valid.drop(e,axis=1)
    return df_train,df_valid
    

def regression_cv(X_train_cf,y_train_cf):
    classifiers = [linear_model.Ridge(alpha=.5),
                   LinearRegression(),
                   linear_model.Lasso(alpha=0.1),
                   tree.DecisionTreeRegressor(),
                   linear_model.LassoLars(alpha=.1),
                   linear_model.BayesianRidge(),
                   svm.SVR(),
                   RandomForestRegressor(n_estimators=100, oob_score=True, random_state=42),
                   KNeighborsRegressor(n_neighbors=5)]
    classifier_accuracy_list = []
    for i, classifier in enumerate(classifiers):
        accuracies = -cross_val_score(classifier, X_train_cf, y_train_cf, cv=5,scoring='neg_mean_squared_error')
        classifier_accuracy_list.append((accuracies.mean(), type(classifier).__name__))

    classifier_accuracy_list = sorted(classifier_accuracy_list, reverse=False)
    for item in classifier_accuracy_list:
        print(item[1], ':', item[0])

def random_Gridsearch_regress():
    n_estimators = [int(x) for x in np.linspace(start = 100, stop = 1000, num = 5)]
    max_features = ['auto', 'sqrt']
    max_depth = [int(x) for x in np.linspace(2, 100, num = 11)]
    max_depth.append(None)
    min_samples_split = [2, 3, 4, 5, 6,7,8,9, 10]
    min_samples_leaf = [1, 2, 4]
    bootstrap = [True, False]
    warm_start = [True, False]
    random_grid = {'n_estimators': n_estimators,
                   'max_features': max_features,
                   'max_depth': max_depth,
                   'min_samples_split': min_samples_split,
                   'min_samples_leaf': min_samples_leaf,
                   'bootstrap': bootstrap,
                   'warm_start':warm_start}
    rf = RandomForestRegressor()
    rf_random = RandomizedSearchCV(estimator = rf, 
                                   param_distributions = 
                                   random_grid, 
                                   n_iter = 20, 
                                   cv = 3, 
                                   verbose=1, 
                                   random_state=8, 
                                   n_jobs = -1,
                                   scoring='neg_mean_squared_error')
    return rf_random


def regression(df_train,df_valid):
    
    col_l = ['cast','crew','genres','keywords','production_companies','spoken_languages','release_date']
    drop_cols = ['homepage','original_title','production_countries',
                 'original_language','overview','status','rating','tagline'] #keep tagline
    col_names = {'cast':50, #30
                 'crew':200, #115
                 'genres':10,
                 'keywords':60,
                 'production_companies':4,
                 'spoken_languages':10
                 }
    train,valid = get_encode_df(df_train,df_valid,col_l,drop_cols,col_names)
    
    X_train = train.loc[:,~train.columns.isin(['movie_id','revenue'])]
    y_train = train['revenue']
    X_test = valid.loc[:,~valid.columns.isin(['movie_id','revenue'])]
    y_test = valid['revenue']

    #regression_cv(X_train,y_train) # use grid to choose a best regression model
    #rf_random = random_Gridsearch_regress().fit(X_train,y_train) #use random grid search find the best param
    reg2 = RandomForestRegressor(n_estimators=200,
                                 warm_start=False,
                                min_samples_split=8,
                                min_samples_leaf=1,
                                max_features='auto',
                                max_depth=100,
                                bootstrap=True).fit(X_train,y_train)

    y_pred2 = np.round(reg2.predict(X_test),decimals=2)
    mse2 = float("{:.2f}".format(mean_squared_error(y_test, y_pred2)))
    coef_score2 = float("{:.2f}".format(np.corrcoef(y_test,y_pred2)[0][1]))
    # print("RandomForest",mse2,coef_score2)

    df_summary = pd.DataFrame({'zid':'z5199285',
                               'MSR':[mse2],
                               'correlation':[coef_score2]})
    
    df_output = pd.DataFrame({'movie_id':valid['movie_id'],'predicted_revenue':y_pred2})
    df_summary.to_csv('z5199285.PART1.summary.csv',index=False)
    df_output.to_csv('z5199285.PART1.output.csv',index=False)

def classifi_cv(X_train_cf,y_train_cf):
    classifiers = [KNeighborsClassifier(),
               DecisionTreeClassifier(),
               LinearDiscriminantAnalysis(),
               LogisticRegression(),
               GaussianNB(),
               SVC(),
               BernoulliNB(),
               MultinomialNB(),
               RandomForestClassifier(n_estimators=100)]
    classifier_accuracy_list = []
    for i, classifier in enumerate(classifiers):
        accuracies = cross_val_score(classifier, X_train_cf, y_train_cf, cv=5,scoring='accuracy')
        classifier_accuracy_list.append((accuracies.mean(), type(classifier).__name__))

    classifier_accuracy_list = sorted(classifier_accuracy_list, reverse=True)
    for item in classifier_accuracy_list:
        print(item[1], ':', item[0])


def random_Gridsearch_class():
    n_estimators = [int(x) for x in np.linspace(start = 200, stop = 1000, num = 5)]
    max_features = ['auto', 'sqrt']
    max_depth = [int(x) for x in np.linspace(2, 100, num = 11)]
    max_depth.append(None)
    min_samples_split = [2, 3, 4, 5, 6,7,8,9, 10]
    min_samples_leaf = [1, 2, 4]
    bootstrap = [True, False]
    warm_start = [True, False]
    random_grid = {'n_estimators': n_estimators,
                   'max_features': max_features,
                   'max_depth': max_depth,
                   'min_samples_split': min_samples_split,
                   'min_samples_leaf': min_samples_leaf,
                   'bootstrap': bootstrap,
                   'warm_start':warm_start}
    rf = RandomForestClassifier()
    rf_random = RandomizedSearchCV(estimator = rf, 
                                   param_distributions = 
                                   random_grid, 
                                   n_iter = 50, 
                                   cv = 3, 
                                   verbose=1, 
                                   random_state=8, 
                                   n_jobs = -1,
                                   scoring='accuracy')
    return rf_random


def GridSearch():
    bootstrap = [True]
    max_depth = [10,20,30, 40, 50]
    max_features = ['auto']
    min_samples_leaf = [1, 2, 3, 4]
    min_samples_split = [2, 3, 4, 5, 10, 15]
    n_estimators = [100,200,300]

    param_grid = {
        'bootstrap': bootstrap,
        'max_depth': max_depth,
        'max_features': max_features,
        'min_samples_leaf': min_samples_leaf,
        'min_samples_split': min_samples_split,
        'n_estimators': n_estimators
    }

    rfc = RandomForestClassifier(random_state=8)

    cv_sets = ShuffleSplit(n_splits = 3, test_size = .33, random_state = 8)

    grid_search = GridSearchCV(estimator=rfc, 
                               param_grid=param_grid,
                               scoring='accuracy',
                               cv=cv_sets,
                               verbose=1)
    return grid_search


def classification(df_train,df_valid):
    col_l = ['cast','crew','genres','keywords','production_companies','spoken_languages','release_date']
    drop_cols = ['homepage','original_title','production_countries',
                 'original_language','overview','tagline','status','revenue']
    col_names = {'cast':100, #30
                 'crew':200, #115
                 'genres':10,
                 'keywords':40,
                 'production_companies':4,
                 'spoken_languages':10}
    train,valid = get_encode_df(df_train,df_valid,col_l,drop_cols,col_names)
    X_train = train.loc[:,~train.columns.isin(['movie_id','rating'])]
    y_train = train['rating']
    X_test = valid.loc[:,~valid.columns.isin(['movie_id','rating'])]
    y_test = valid['rating']
    #classifi_cv(X_train,y_train) #choose randomForest as our classifier
    #rf_random = random_Gridsearch_class() #use random grid search find the best param
    
    clf = RandomForestClassifier(n_estimators=200,
                                 warm_start=False,
                                min_samples_split=2,
                                min_samples_leaf=3,
                                max_features='auto',
                                max_depth=30,
                                bootstrap=True).fit(X_train,y_train)
#     grid_search = GridSearch() #use grid search to further tune the hyperparameter
    y_predict = clf.predict(X_test)
    y_train_predict = clf.predict(X_train)
    cm = confusion_matrix(y_test,y_predict)
    prec_score = float("{:.2f}".format(precision_score(y_test,y_predict,average='macro')))
    recal_score = float("{:.2f}".format(recall_score(y_test,y_predict,average='macro')))
    acc_score = float("{:.2f}".format(accuracy_score(y_test,y_predict)))
    # print(prec_score,recal_score,acc_score)
    # print(accuracy_score(y_train,y_train_predict))
    # print(cm)
    df_summary = pd.DataFrame({'zid':'z5199285',
                               'average_precision':[prec_score],
                               'average_recall':[recal_score],
                               'accuracy':[acc_score]})
    
    df_output = pd.DataFrame({'movie_id':valid['movie_id'],'predicted_rating':y_predict})
    df_summary.to_csv('z5199285.PART2.summary.csv',index=False)
    df_output.to_csv('z5199285.PART2.output.csv',index=False)




if __name__ == '__main__':
    train_file = sys.argv[1]
    validation_file = sys.argv[2]
    df_train = pd.read_csv(train_file)
    df_valid = pd.read_csv(validation_file)
    regression(df_train,df_valid)
    classification(df_train,df_valid)
    

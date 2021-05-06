import submission
import pickle
import time

with open('./example/Query_File_1', 'rb') as f:
    Query_File = pickle.load(f, encoding = 'bytes')
with open('./example/CodeBooks_1', 'rb') as f:
    codebooks = pickle.load(f, encoding = 'bytes') #shape 2,256,64
with open('./example/Codes_1', 'rb') as f:
    codes = pickle.load(f, encoding = 'bytes') #shape 2,256,64
    
queries = Query_File#pickle.load(Query_File, encoding = 'bytes')
queries = queries.reshape(10,128)
start = time.time()
candidates = submission.query(queries, codebooks, codes, T=10)
end = time.time()
time_cost_2 = end - start
print(time_cost_2)
print(candidates)
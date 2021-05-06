from scipy.spatial import distance
import numpy as np
import queue as Q

def K_star(data,centroids,max_iter):
    k = 1
    c_shape0 = centroids.shape[0]
    codes = np.empty((data.shape[0],1),dtype=np.uint8)
    codes[:] = np.NaN
    centroids = centroids.astype(np.float32)
    while k <= max_iter:     
        for i in range(data.shape[0]):
            d_manht = distance.cdist([data[i]],centroids,'cityblock')
            index = np.argmin(d_manht)
            codes[i] = index
        
        for j in range(c_shape0):
            lk = np.argwhere(codes == j)
            if lk.size > 0:
                cluster_list = [data[x[0]] for x in lk]
                centroids[j] = np.median(cluster_list,axis=0)
        k+=1
    for i in range(data.shape[0]):
            d_manht = distance.cdist([data[i]],centroids,'cityblock')
            index = np.argmin(d_manht)
            codes[i] = index
    return codes, centroids

def pq(data, P, init_centroids, max_iter):
    offset = init_centroids.shape[2]
    j = 0
    k = offset
    a = []
    b = []
    for i in range(P):
        data_x = data[:,j:k*(i+1)]
        codes, codebooks = K_star(data_x,init_centroids[i],max_iter)
        a.append(codebooks)
        b.append(codes)
        j = k*(i+1)
    codes = np.concatenate(b,axis=1)
    codebooks = np.array(a)
    return codebooks,codes 

def traverse(traverse_arr,p,matrix_index,ll,x):
    isTrue = True
    for g in range(x):
        cl = [i for i in tuple(ll)]
        if g != p:
            cl[g] = cl[g] - 1  ##some problem here
            if matrix_index[g]==0 or tuple(cl) in traverse_arr:
                isTrue = True
            else:
                isTrue = False
                return isTrue
    return isTrue

def multi_seq(dist_matrix,dist_index_matrix,d_codes,T):
    x = dist_matrix.shape[0]  
    y = dist_matrix.shape[1]  
    z = dist_matrix.shape[2]  
    
    points_list = []
    
    for i in range(y):
        traverse_arr = tuple()
        queue = Q.PriorityQueue()
        points_set = set()
        queue.put((sum(dist_matrix[:,i,0]),(0,)*x)) 
        while len(points_set) < T:
            centroid = queue.get()
            matrix_index = centroid[1]
            traverse_arr += (matrix_index,)
            tt = tuple(dist_index_matrix[j,i,matrix_index[j]] for j in range(x))
            if tt in d_codes:
                points_set.update(d_codes[tt])
            for p in range(x):
                if matrix_index[p] < z:
                    ll = list(matrix_index)
                    ll[p] = ll[p] + 1
                    cl = tuple(c for c in ll)
                    if traverse(traverse_arr,p,matrix_index,ll,x):
                        queue.put((sum([dist_matrix[g,i,cl[g]] for g in range(x)]),cl))
                
        points_list.append(points_set)
    return points_list                   
        
        
def query(queries,codebooks,codes,T): 
    j = 0 
    dist_matrix = []
    dist_index_matrix = []
    d_codes = {}
    for i in range(len(codes)):
        tp = tuple(codes[i])
        if tp in d_codes:
            d_codes[tp].update([i])
        else:
            d_codes[tp] = set()
            d_codes[tp].update([i])
    for e in codebooks:
        i = j
        j += e.shape[1]
        sub_query = queries[:,i:j]
        d = distance.cdist(sub_query,e,'cityblock')
        sort_d = np.sort(d)
        argsort_d = np.argsort(d)
        dist_matrix.append(sort_d)
        dist_index_matrix.append(argsort_d)
    dist_matrix = np.array(dist_matrix)
    dist_index_matrix = np.array(dist_index_matrix)
    points_list = multi_seq(dist_matrix,dist_index_matrix,d_codes,T)
    return points_list
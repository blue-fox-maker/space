import copy
from networkx.algorithms import bipartite
import networkx as nx
import operator 
import sys 
import itertools 
from rich import print 
from collections import defaultdict 
from functools import partial 
from rich.console import Console
import math
G = nx.Graph() 
k = 3 
t_min, t_max = sys.maxsize, 0 
trivial_partition = set()  
path = "data/" 
path += input("input dataset file: ")  
console = Console()  
with console.status("Loading File "+path+"..."): 
    with open(path,"r") as file: 
        for line in file: 
            t,u,v = list(map(int,line.split(' '))) 
            G.add_edge(u,v,time_stamp = t)
            t_min = min(t, t_min)
            t_max = max(t, t_max)
            trivial_partition.add(frozenset({u}))
            trivial_partition.add(frozenset({v}))
t_max = t_max+1
console.log("Read File Finished")

with console.status("Computing Core Value..."):
    result = defaultdict(set)
    for ts in range(t_min,t_max):
        for te in range(ts, t_max):
            projected_graph = nx.subgraph_view(G,filter_edge = lambda u,v: ts<=G[u][v].get('time_stamp')<=te)
            k_core_set = set()
            for v,degeneracy in nx.core_number(projected_graph).items():
                if degeneracy >= k:
                    k_core_set.add(v)
                else:
                    result[ts,te].add(frozenset({v}))
            if len(k_core_set)!=0:
                result[ts,te].add(frozenset(k_core_set))
console.log("Core Value Compute Finished")

A = defaultdict(set)
with console.status("Collecting A..."):
    for ts in range(t_min, t_max):
        result[ts,ts-1] = trivial_partition        
        for te in range(ts,t_max):
            for equivalence_class in result[ts,te]:
                sub_part = []
                for sub_equivalence_class in result[ts,te-1]:
                    if sub_equivalence_class.issubset(equivalence_class):
                        sub_part.append(sub_equivalence_class)
                acc_sub_part = list(itertools.accumulate(sub_part,operator.or_))
                for i in range(len(sub_part)-1):
                    A[ts,te].add((acc_sub_part[i], sub_part[i+1]))
console.log("A collected")
console.print(f"size of A: {len(A)}")

cover_dict = {}
with console.status("Generating Cover Dict..."):
    for key,value in A.items():
        ts,te = key
        for part1, part2 in value:
            a = (part1,part2,ts,te)
            for u in part1:
                for v in part2:
                    cover_dict[(ts,ts),u,v,te] = [a]
console.log("Cover Dict Generated")

compact_cover_dict = {}
with console.status("Compressing Cover Dict..."):
    for key in sorted(cover_dict.keys()):
        value = cover_dict[key]
        interval,u,v,t_hat = key
        ts,te = interval
        if value != None:
            compact_cover_dict[key] = value
            while ((te+1,te+1),u,v,t_hat) in cover_dict.keys():
                compact_cover_dict[(ts,te+1),u,v,t_hat] = compact_cover_dict[(ts,te),u,v,t_hat] + cover_dict[(te+1,te+1),u,v,t_hat]
                cover_dict[(te+1,te+1),u,v,t_hat] = None
                del compact_cover_dict[(ts,te),u,v,t_hat]
                te = te+1
        
console.log("Compact Cover Dict Generated")
console.log(len(compact_cover_dict))

bigraph = nx.Graph()
with console.status("Building Bipartite Graph..."):
    bigraph.add_node("virtual node", bipartite = 0)
    for key, values in compact_cover_dict.items():
        bigraph.add_node(key,bipartite = 1)
        bigraph.add_edge(key,"virtual node")
        for value in values:
            bigraph.add_node(value,bipartite = 0)
            bigraph.add_edge(key,value)

console.log("Bipartite Graph Generated")
console.log(bigraph)

result = []
with console.status("Computing Maximum Matching..."):
    matching = nx.bipartite.maximum_matching(bigraph)
    for u in matching.keys():
        if bigraph.nodes[u]["bipartite"] ==1:
            result.append(u)
console.log("Maximum Matching Generated")

lne_index = defaultdict(list)
with console.status("Generating Lne Index..."):
    for interval, u,v,t_hat in result:
        t1,t2 = interval
        lne_index[t1,t2].append((u,v,t_hat))

console.log(lne_index)
console.log("Lne Index Generated")

def query(ts,te):
    res = set()
    for interval, values in lne_index.items():
        t1,t2 = interval
        if t1<=ts<=t2<=te:
            for u,v,t_hat in values:
               if ts<=t_hat<=te:
                    res.add(u)
                    res.add(v)
    return res

while True:
    interval = console.input("query interval: ")
    ts,te = tuple(int(x) for x in interval.split())
    console.log(query(ts,te))
    projected_graph = nx.subgraph_view(G,filter_edge = lambda u,v: ts<=G[u][v].get('time_stamp')<=te)
    console.log("ground truth: " + str(nx.k_core(projected_graph,k).nodes))


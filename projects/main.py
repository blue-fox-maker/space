import networkx as nx
import argparse
import pandas as pd
import itertools
import operator
from collections import defaultdict
from rich.console import Console
from math import floor

# parser = argparse.ArgumentParser()
# parser.add_argument("dataset")
# parser.add_argument("-k",type=int)
# args = parser.parse_args()
console = Console()

# df = pd.read_csv(args.dataset, sep=" ", names=["source","target","timestamp"],dtype=int)
# df.sort_values("timestamp")
# df["timestamp"] = pd.to_datetime(df["timestamp"],unit="s").dt.date
# df["timestamp"] = df.groupby("timestamp").ngroup()
# df["timestamp"] = df["timestamp"].apply(lambda x: x//5)
# G = nx.from_pandas_edgelist(df,edge_attr=["timestamp"])
# print(df)
# G.graph["lifespan"] = len(df["timestamp"].unique())
# console.log(f"Load {G} in [0,{G.graph['lifespan']})")
# t_min = 0
# t_max = G.graph["lifespan"]

# result = defaultdict(set)
# for ts in range(t_min,t_max):
#     for te in range(ts,t_max):
#         projected_graph = nx.subgraph_view(G,filter_edge = lambda u,v: ts<=G[u][v]["timestamp"]<=te)
#         k_core_set = set()
#         for v, degeneracy in nx.core_number(projected_graph).items():
#             if degeneracy >= args.k:
#                 k_core_set.add(v)
#             else:
#                 result[ts,te].add(frozenset({v}))
#         if len(k_core_set)!=0:
#             result[ts,te].add(frozenset(k_core_set))

# A = defaultdict(set)
# trivial_partition = {frozenset({v}) for v in G.nodes}
# for ts in range(t_min,t_max):
#     result[ts,ts-1] = trivial_partition
#     for te in range(ts,t_max):
#         for equivalence_class in result[ts,te]:
#             sub_part = []
#             for sub_equivalence_class in result[ts,te-1]:
#                 if sub_equivalence_class.issubset(equivalence_class):
#                     sub_part.append(sub_equivalence_class)
#             acc_sub_part = list(itertools.accumulate(sub_part,operator.or_))
#             for i in range(len(sub_part)-1):
#                 A[ts,te].add((acc_sub_part[i], sub_part[i+1]))
# console.log(f"Load A of size {sum(map(len,A.values()))}")

# cover_dict = {}
# for key,value in A.items():
#     ts,te = key
#     for part1,part2 in value:
#         a = (part1,part2,ts,te)
#         for u in part1:
#             for v in part2:
#                 cover_dict[u,v,ts,te] = a
# console.log(f"Load cover dict of size {len(cover_dict)}") 

# compact_cover_dict = defaultdict(list)
# for key in sorted(cover_dict.keys(),key= lambda x:(x[2],x[3])):
#     u,v,ts,t_hat = key
#     te = ts
#     if cover_dict[key] != None:
#         covered = [] 
#         cur_t = ts
#         while (u,v,cur_t,t_hat) in cover_dict.keys():
#             te = cur_t
#             covered.append(cover_dict[u,v,cur_t,t_hat])
#             cover_dict[u,v,cur_t,t_hat] = None
#             cur_t = cur_t +1
#         compact_cover_dict[u,v,ts,te,t_hat] = covered
# console.log(f"Load compact cover dict of size {len(compact_cover_dict)}")

# bigraph = nx.Graph()
# for key,values in compact_cover_dict.items():
#     for value in values:
#         bigraph.add_edge(key,value)
# console.log(f"Generate bipartite {bigraph}")

lne_index = []
# matching = nx.bipartite.maximum_matching(bigraph,top_nodes=compact_cover_dict.keys())
# for v in matching.keys():
#     if v in compact_cover_dict.keys():
#         lne_index.append(v)
# console.log(f"Compute index of size {len(lne_index)}")

G = nx.Graph()
L = []
with open("lne_graph.txt","r") as file:
    for line in file:
        x,y,ts,te,u,v,t1,t2,t3 = tuple(map(int,line.split()))
        G.add_edge((x,y,ts,te),(u,v,t1,t2,t3))
        L.append((x,y,ts,te))
matching = nx.bipartite.maximum_matching(G,top_nodes=L)
for v in matching.keys():
    if v not in L:
        lne_index.append(v)

def query(ts,te):
    res = set()
    for u,v,t1,t2,t_hat in lne_index:
        if t1<=ts<=t2<=te and ts<=t_hat<=te:
            res.add(u)
            res.add(v)
    return res

res_index = defaultdict(list)
for u,v,ts,te,t_hat in lne_index:
    res_index[ts,te].append((u,v,t_hat))
console.print(res_index)
console.rule("query")

while True:
    interval = console.input("query interval: ")
    ts,te = tuple(int(x) for x in interval.split())
    console.log(query(ts,te))
    # projected_graph = nx.subgraph_view(G,filter_edge = lambda u,v: ts<=G[u][v].get('timestamp')<=te)
    # console.log(f"ground truth: {set(nx.k_core(projected_graph,args.k).nodes)}")

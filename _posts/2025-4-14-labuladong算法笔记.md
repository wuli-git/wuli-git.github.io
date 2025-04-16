---
title: labuladong
tags: TeXt
---

# 数据结构

## 1.vector

```cpp
#include <vecctor>
vector<int>a;
vector<int>a(n,0);
vector<vector<int> >b;
vector<vector<int> >b(n,vector<int>(5,0));
a.push_back(666);//插入
a.insert(a.begin()+2,e);
a.erase(a.begin()+2);//删除

a.size();
a.empty();

a.front();
a.back();
```

## 2.stack

```cpp
#include <stack>
stack<int>stk;
stk.push(4);
stk.pop();
stk.top();

stk.empty();
stk.size();
```

## 3.queue

```cpp
#include <queue>
queue<int>Q;
Q.push(4);
Q.pop();

Q.front();
Q.back();

Q.empty();
Q.size();

```

## 4.unordered_set()()(哈希集合，后文有更详细介绍)

```cpp
#include <unordered_set>
unordered_set<int>Table;
Table.insert(e);
Table.erase(e);
Table.emplace(e);//插入，比insert高效

Table.count(e);
Table.find(e);

Table.empty();
Table.size();
Table.clear();

for(auto &x: Table)cout<<x<<endl;
```

## 5.string

```cpp
#include <string>
string s1;
s1="apple";
string s2("hello");
string s3(s2);
string s4(5,'A');
s=s1+s2;
s+="app";
reverse(s.begin(),s.end());
sort(s.begin(),s.end);

s.substr(2,3);//从第二个位置截取3个字符

stoi();//将字符型转化为整数类型
stoi(s,nullptr,2);//将字符型的十进制的数转化为2进制的数
stod(s);//将字符型转成double类型
```


## 6.priority_queue;

- priority_queue是大根堆，里面的父节点永远比子节点要大，父节点是i,左子节点是2i+1,右子节点是2i+2(从0开始)，这是用队列来模拟

```cpp
struct cmp{
    bool operator()(int &a,int &b)const{
        return a>b;//比较函数，小根堆，第三个参数是一个类型，所以cmp不能直接写成一个函数,当比较函数返回 true 时，意味着第一个参数的优先级低于第二个参数，也就是第二个参数会排在前面，b在前面
    }
};
#include <queue>
using namespace std;
priority_queue<int,vector<int>,cmp>pq;//类型，容器，比较类型
pq.push(1);
pq.push(2);
while(!pq.empty()){
    cout<<pq.top()<<endl;
    pq.pop();//弹出
}
```

## 8.哈希表和哈希集合

- 哈希表：散列表，键值对，key:value;通过哈希函数将key转化为数组的索引，再把value存在那个索引上

```plaintext
// 哈希表伪码逻辑
class MyHashMap {

private:
    vector<void*> table;

public:
    // 增/改，复杂度 O(1)
    void put(auto key, auto value) {
        int index = hash(key);
        table[index] = value;
    }

    // 查，复杂度 O(1)
    auto get(auto key) {
        int index = hash(key);
        return table[index];
    }

    // 删，复杂度 O(1)
    void remove(auto key) {
        int index = hash(key);
        table[index] = nullptr;
    }

private:
    // 哈希函数，把 key 转化成 table 中的合法索引
    // 时间复杂度必须是 O(1)，才能保证上述方法的复杂度都是 O(1)
    int hash(auto key) {
        // ...
    }
};
```

##### 哈希函数

- 1.int hashCode()会返回该对象全局的内存地址
- 哈希冲突:出现了不同的key映射到同一个地址上；
- (1)拉链法:拉链法相当于是哈希表的底层数组并不直接存储 value 类型，而是存储一个链表，当有多个不同的 key 映射到了同一个索引上，这些 key -> value 对儿就存储在这个链表中，这样就能解决哈希冲突的问题。
- (2)而线性探查法的思路是，一个 key 发现算出来的 index 值已经被别的 key 占了，那么它就去 index + 1 的位置看看，如果还是被占了，就继续往后找，直到找到一个空的位置为止

### 哈希集合unordered_set

- 实质就是哈希表的键，只有key然后去对照数组的索引

```cpp
#include <unordered_set>
using namespace std;
unordered_set<int>set;
for(int i:shuzu){
    set.insert(i);//插入操作
}
for(const auto&element:set){
    cout<<element<<endl;//遍历
}
auto it=set.find(20);
if(it!=set.end()){
    return true;
}else{
    return false;//查找
}
```

- 例题：通过哈希表来给出一个数组a中每个数字出现了几次，将数字为key直接为unorder_set的索引，出现次数为value

```cpp
vector<int>hash;
//memset(hash,0,sizeof(hash));//这是不正确的
fill(hash.begin(),hash.end(),0);//初始化
for(int i=0;i<a.size();i++){
    hash[a[i]]++;
}
for(int i=0;i<hash.size();i++){
    cout<<i<<"<<"<<hash[i];//输出每一个值以及出现了几次
}
```

## 9并查集fa[N]

- 作用:并查集可以高效的对元素进行分组（合并在一起），并且能快速的查询两个元素是否属于同一组。

- 并查集是一种树状结构，一个集合有一个根节点，以后每加入的元素其都会连接到这个树结构上，判断链各个元素是否在一个集合，找其父节点最终找到其根节点，看是不是同一个

```cpp
int fa[N];
for(int i=0;i<N;i++){
    fa[i]=i;//初始化并查集，将并查集中的每个元素都指向自己
}
int find(int x){
    if(x==f[x])return x;
    return find(fa[x]);//并查集查找函数，找每个并查集的根节点，从而判断是不是在一个集合
}
void merge(){
    int x=find(i);
    int y=find(j);
    fa[x]=y;//合并并查集
}
```

# 算法

## 图论

### 求最小生成树

- kruskal加边法，将边排序，从最小的开始加（贪心思想）不形成环，直到形成最小生成树

```cpp
#include <bits/stdc++.h>//万能头文件
using namespace std;
const int N=1000;
int fa[N];
int find(int x){
    if(x==fa[x]){
        return x;
    }
    return find(fa[x]);
}
void init(){
    for(int i=0;i<N;i++){
        fa[i]=i;
    }
}
struct edge{
    int u,v,w;
}e[N];//定义边的结构体
bool cmp(edge a,edge b){
    return a.w<b.w;
}
int kruskal(int n,int m){
    sort(e,e+m,cmp);
    int ans=0;
    int count=0;
    for(int i=0;i<m;i++){
        int x=find(e[i].u);
        int y=find(e[i].v);
        if(x!=y){
            fa[y]=x;
            count++;
            ans+=e[i].w;
        }
    }
    if(count==n-1)return ans;
    else{
        return -1;
    }
}
int main()
{
    int n,m;
    cin>>n>>m;
    for(int i=0;i<m;i++){
        cin>>e[i].u>>e[i].v>>e[i].w;
    }
    int ans=kruskal(n,m);
    cout<<ans<<endl;
}
```
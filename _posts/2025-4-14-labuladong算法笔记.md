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

## 4.unordered_set()

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
---
title: labuladong algorithm 笔记
tags: TeXt
---

# 数据结构

## 目录

- # 数据结构与算法目录

[一、数据结构](#一数据结构)  

- [1. vector](#1vector)  
- [2. stack](#2stack)  
- [3. queue](#3queue)  
- [4. unordered_set（哈希集合）](#4unordered_set哈希集合)  
- [5. string](#5string)  
- [6. priority_queue（优先队列）](#6priority_queue优先队列)  
- [7. 链表（List）](#7链表list)  
  - [7.1 链表基本操作](#71链表基本操作)  
  - [7.2 链表相关算法问题](#72链表相关算法问题)  
- [8. 哈希表和哈希集合](#8哈希表和哈希集合)  
  - [8.1 哈希表基本概念](#81哈希表基本概念)  
  - [8.2 哈希函数与哈希冲突](#82哈希函数与哈希冲突)  
  - [8.3 哈希集合（unordered_set）](#83哈希集合unordered_set)  
- [9. 并查集（fa [N]）](#9并查集fan)  
- [10. 红黑树](#10红黑树)  

[二、算法](#二算法)  

- [1. DFS（深搜 / 回溯）](#1dfs深搜-回溯)  
  - [1.1 DFS 基本概念与模板](#11dfs-基本概念与模板)  
  - [1.2 DFS 应用实例](#12dfs-应用实例)  
    - [1.2.1 数独问题](#121数独问题)  
    - [1.2.2 n 位二进制数的全排列](#122n-位二进制数的全排列)  
    - [1.2.3 凑数问题（如凑 24）](#123凑数问题如凑-24)  
- [2. 走迷宫问题与矩阵中的最长递增路径](#2走迷宫问题与矩阵中的最长递增路径)  
  - [2.1 走迷宫问题概述](#21走迷宫问题概述)  
  - [2.2 矩阵中的最长递增路径问题描述与解法](#22矩阵中的最长递增路径问题描述与解法)  
- [3. 图论相关算法](#3图论相关算法)  
  - [3.1 最大流问题](#31最大流问题)  
  - [3.2 最短路径问题（Dijkstra 算法）](#32最短路径问题dijkstra-算法)  
  - [3.3 最小生成树问题（Kruskal 算法与 Prim 算法）](#33最小生成树问题kruskal-算法与-prim-算法)  
- [4. 动态规划](#4动态规划)  
  - [4.1 0-1 背包问题](#410-1-背包问题)  
  - [4.2 多重背包问题](#42多重背包问题)  

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

## 6.priority_queue

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

## 7链表List

```cpp
//创建一个链表节点 class方法 struct方法
class ListNode{
public:
    int val;
    ListNode next*;
    ListNode(int x):val(x),next(NULL){}
};
struct ListNode{
    int val;
    ListNode next*;
    ListNode(int x):val(x),next(NULL){}
};
//创建一个单链表,
ListNode*createLinkedList(vector<int>arr){//通过一个数组创键
    if(arr.empty())return NULL;
    ListNode*head=new ListNode(arr[0]);
    ListNode* cur=head;
    for(int i=0;i<arr.size();i++){
        cur->next=new ListNode(arr[i]);
        cur=cue->next;
    }
    return head;
}
ListNode *createinputList(void){//靠输入来创建
    int x;
    cin>>x;
    ListNode *head=new ListNode(x);
    ListNode *cur=head;
    while((cin>>x)&&x!=-1){
        cur->next=new ListNode(x);
        cur=cur->next;
    }
    return head;
}
void traverse(ListNode *head){
    for(ListNode*p=head;p!=NULL;p=p->next){
        cout<<p->val<<" ";
    }
}
//增
ListNode *addElement(ListNode* head,int e){
    ListNode*p=head;
    while(p->next!=NULL)p=p->next;
    p->next=new ListNode(e);
    return head;
}
//在中间插入元素
ListNode *addzhongjianList(ListNode head,int de,int val){
    ListNode* p=head;
    for(int i=0;i<de;i++){
        p=p->next;
    }
    ListNode *Newnode=new ListNode(val);
    Newnode->next=p->next;
    p->next=Newnode;
    return head;
}
//删除一个节点
ListNode* deleteNode(ListNode *head,int de){
    ListNode*p=head;
    for(int i=0;i<de-1;i++){
        p=p->next;
    }
    p->next=p->next->next;
    return head;
}
```

### 7.2有关链表算法问题

- 7.2.1合并两个有序链表

```cpp
ListNode *mergeList(ListNode *l1,ListNode *l2){
    ListNode *dummy(-1),*p=&dummy;//虚拟头结点
    ListNode *p1=l1,*p2=l2;
    while(p1!=nullptr&&p2!=nullptr){
        if(p1->val<p2->val){
            p->next=p1;
            p1=p1->next;
        }else{
            p->next=p2;
            p2->next=p2;
        }
    }
    if(p1!=nullptr){
        p->next=p1;
    }
    if(p2!=nullptr){
        p->next=p2;
    }
    return dummy->next;//返回虚拟节点的下一个节点
}
```

- 7.2.2环形链表

- 快慢指针,fast,slow

```cpp

```

- 7.2.3环形列表（需要求出从哪个节点开始环）

- 用快慢指针，当快慢指针在同一位置时，将其中一个指针指向head，以相同速度向后走，当再次相遇时的节点是要求的节点

```cpp
ListNode *detectCycle(ListNode *head) {
        ListNode *fast=head,*slow=head;
        while(fast!=nullptr&&fast->next!=nullptr){
            fast=fast->next->next;
            slow=slow->next;
            if(fast==slow){
                break;
            }
        }
        if(fast==nullptr||fast->next==nullptr){
            return NULL;
        }
        slow=head;
        while(slow!=fast){
            fast=fast->next;
            slow=slow->next;
        }
        return fast;
    }
```

#### 7.2.4

- 分割链表:给出一个链表List和一个关键字x，将比x小和比x大的分开，最后再连成一条链，小的在x左边，大的在右边

```cpp
ListNode partitio(ListNode *head,int x){
    ListNode *dummy1=new ListNode(-1),*p1=dummy1;
    ListNode *dummy2=new ListNode(-1),*p2=dummy2;
    ListNode *p=head;
    while(p!=nullptr){
        if(p->val<x){
            p1->next=p;
            p1=p1->next;
        }else{
            p2->next=p;
            p2=p2->next;
        }
        ListNode *temp=p->next;
        p->next=nullptr;
        p=temp;
    }
    p1->next=dummy2->next;
    return dummy1->next;
}
```

#### 7.2.5.合并k个链表

- 这里要用到优先级队列这种数据结构，把链表节点放入一个最小堆，就可以每次获得 k 个节点中的最小节点

```cpp
struct cmp{
    bool operator()(const ListNode *a,const ListNode *b){
        return a->val>b->val;
    }
};//priority_queue接受的cmp参数是一个结构体参数
ListNode *mergeKList(vector<ListNode *>&lists){
    if(lists.empty())return nullptr;
    ListNode *dummy=new ListNode(-1),*p=dummy;
    priority_queue<ListNode*,vector<ListNode*>,cmp>pq;
    for(ListNode *head:lists){//将每个链表的头结点加入到pq中
        if(head!=nullptr){
            pq.push_back(head);
        }
    }
    while(!pq.empty()){
        p->next=pq.top();
        pq.pop();
        if(pq->next->next!=nullptr)pq.push_back(pq->next->next);
        p=p->next;//要知道是把哪一个节点加入到pq中
    }
    return dummy->next;
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
set.erase(val);//删除
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

## 9.并查集fa[N]

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

## 10.红黑树

红黑树（Red-Black Tree）是一种自平衡的二叉搜索树，它在计算机科学中被广泛应用，尤其是在需要高效存储和检索有序数据的场景中。以下是关于红黑树的详细介绍：1. 定义与性质红黑树是每个节点都带有颜色属性的二叉搜索树，颜色通常为红色或黑色。

- 除了具备二叉搜索树的基本性质（左子树的所有节点值小于根节点值，右子树的所有节点值大于根节点值）外，还满足以下性质：

- 每个节点要么是红色，要么是黑色。根节点是黑色。

- 每个叶子节点（NIL 节点，空节点）是黑色。

- 如果一个节点是红色的，那么它的两个子节点都是黑色的（即不存在连续的红色节点）。

- 从任一节点到其每个叶子节点的所有路径都包含相同数目的黑色节点（称为黑高）。

2. 优势与应用高效的插入和删除操作：普通的二叉搜索树在最坏情况下可能退化为链表，导致插入、删除和查找操作的时间复杂度为 \(O(n)\)。而红黑树通过维持上述性质，保证了在最坏情况下，这些操作的时间复杂度为 \(O(\log n)\)，其中 n 是树中节点的数量。有序数据的存储与检索：由于红黑树是二叉搜索树，它能很好地存储有序数据，常用于实现关联数组、字典等数据结构，例如 C++ 标准库中的 std::map 和 std::set 底层就可以基于红黑树实现。数据库索引：在数据库系统中，红黑树可以用于实现索引，加快数据的查询速度。

3. 节点结构示例（以 C++ 代码表示）

```cpp
struct node{
    int key;  // 节点存储的值
    bool color;  // 节点颜色，true 表示红色，false 表示黑色
    Node* left;
    Node* right;
    Node* parent;

    Node(int k) : key(k), color(true), left(nullptr), right(nullptr), parent(nullptr) {}
};
```

4. 插入操作插入新节点时，一般先按照二叉搜索树的规则插入，然后将新节点染成红色。接着，通过一系列的颜色调整和旋转操作（左旋、右旋）来恢复红黑树的性质。例如，若是插入根节点，直接把根节点染成黑色，如果新插入节点的父节点是红色，且父节点的兄弟节点也是红色，那么可能需要重新染色（将父节点和叔节点和爷节点进行交换）；如果父节点是红色且兄弟节点是黑色，可能需要进行旋转操作。（旋转之后对旋转的节点进行交换）

5. 删除操作删除节点相对复杂一些。首先按照二叉搜索树的删除规则删除节点，然后根据被删除节点和其替代节点的颜色情况，进行相应的调整操作，以确保红黑树的性质仍然满足。可能会涉及到重新染色和旋转操作，以保持树的平衡和颜色性质。

```cpp
#include <iostream>

// 定义红黑树节点颜色
enum Color { RED, BLACK };

// 红黑树节点结构
struct Node {
    int data;
    bool color;
    Node* left, * right, * parent;

    Node(int data) : data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

// 红黑树类
class RedBlackTree {
private:
    Node* root;

    // 左旋操作
    void rotateLeft(Node*&, Node*&);
    // 右旋操作
    void rotateRight(Node*&, Node*&);
    // 插入修复
    void fixInsert(Node*&, Node*&);
    // 查找最小节点
    Node* minimum(Node* node);
    // 删除修复
    void fixDelete(Node*&, Node*);
    // 辅助删除函数
    void deleteNodeHelper(Node*&, Node*);
    // 中序遍历辅助函数
    void inorderHelper(Node*);

public:
    RedBlackTree() : root(nullptr) {}

    // 插入操作
    void insert(int data);
    // 删除操作
    void deleteNode(int data);
    // 查找操作
    Node* search(int data);
    // 中序遍历
    void inorder();
};

// 左旋操作
void RedBlackTree::rotateLeft(Node*& root, Node*& x) {
    Node* y = x->right;
    x->right = y->left;

    if (y->left != nullptr) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

// 右旋操作
void RedBlackTree::rotateRight(Node*& root, Node*& y) {
    Node* x = y->left;
    y->left = x->right;

    if (x->right != nullptr) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == nullptr) {
        root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

// 插入修复
void RedBlackTree::fixInsert(Node*& root, Node*& z) {
    Node* parent_z = nullptr;
    Node* grand_parent_z = nullptr;

    while ((z != root) && (z->color != BLACK) && (z->parent->color == RED)) {
        parent_z = z->parent;
        grand_parent_z = z->parent->parent;

        // 情况 A：父节点是祖父节点的左子节点
        if (parent_z == grand_parent_z->left) {
            Node* uncle_z = grand_parent_z->right;

            // 情况 1：叔叔节点是红色
            if (uncle_z != nullptr && uncle_z->color == RED) {
                grand_parent_z->color = RED;
                parent_z->color = BLACK;
                uncle_z->color = BLACK;
                z = grand_parent_z;
            } else {
                // 情况 2：叔叔节点是黑色，z 是父节点的右子节点
                if (z == parent_z->right) {
                    rotateLeft(root, parent_z);
                    z = parent_z;
                    parent_z = z->parent;
                }

                // 情况 3：叔叔节点是黑色，z 是父节点的左子节点
                rotateRight(root, grand_parent_z);
                std::swap(parent_z->color, grand_parent_z->color);
                z = parent_z;
            }
        } 
        // 情况 B：父节点是祖父节点的右子节点
        else {
            Node* uncle_z = grand_parent_z->left;

            // 情况 1：叔叔节点是红色
            if ((uncle_z != nullptr) && (uncle_z->color == RED)) {
                grand_parent_z->color = RED;
                parent_z->color = BLACK;
                uncle_z->color = BLACK;
                z = grand_parent_z;
            } else {
                // 情况 2：叔叔节点是黑色，z 是父节点的左子节点
                if (z == parent_z->left) {
                    rotateRight(root, parent_z);
                    z = parent_z;
                    parent_z = z->parent;
                }

                // 情况 3：叔叔节点是黑色，z 是父节点的右子节点
                rotateLeft(root, grand_parent_z);
                std::swap(parent_z->color, grand_parent_z->color);
                z = parent_z;
            }
        }
    }
    root->color = BLACK;
}

// 查找最小节点
Node* RedBlackTree::minimum(Node* node) {
    while (node->left != nullptr)
        node = node->left;
    return node;
}

// 删除修复
void RedBlackTree::fixDelete(Node*& root, Node* x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft(root, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotateRight(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotateLeft(root, x->parent);
                x = root;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight(root, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotateLeft(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotateRight(root, x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

// 辅助删除函数
void RedBlackTree::deleteNodeHelper(Node*& root, Node* z) {
    Node* y = z;
    Node* x;
    bool y_original_color = y->color;

    if (z->left == nullptr) {
        x = z->right;
        transplant(root, z, z->right);
    } else if (z->right == nullptr) {
        x = z->left;
        transplant(root, z, z->left);
    } else {
        y = minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK) {
        fixDelete(root, x);
    }
}

// 中序遍历辅助函数
void RedBlackTree::inorderHelper(Node* node) {
    if (node == nullptr) return;

    inorderHelper(node->left);
    std::cout << node->data << " ";
    inorderHelper(node->right);
}

// 插入操作
void RedBlackTree::insert(int data) {
    Node* z = new Node(data);
    Node* y = nullptr;
    Node* x = root;

    while (x != nullptr) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (y == nullptr)
        root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;

    fixInsert(root, z);
}

// 删除操作
void RedBlackTree::deleteNode(int data) {
    Node* z = search(data);
    if (z != nullptr) {
        deleteNodeHelper(root, z);
        delete z;
    }
}

// 查找操作
Node* RedBlackTree::search(int data) {
    Node* temp = root;
    while (temp != nullptr && temp->data != data) {
        if (data < temp->data)
            temp = temp->left;
        else
            temp = temp->right;
    }
    return temp;
}

// 中序遍历
void RedBlackTree::inorder() {
    inorderHelper(root);
    std::cout << std::endl;
}

int main() {
    RedBlackTree tree;

    tree.insert(7);
    tree.insert(6);
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(2);
    tree.insert(1);

    std::cout << "Inorder traversal after insertion: ";
    tree.inorder();

    tree.deleteNode(5);

    std::cout << "Inorder traversal after deletion of 5: ";
    tree.inorder();

    return 0;
}    
```

# 二、算法

## 2.1、DFS(深搜/回溯)

- 从图或树的某个起始节点开始，沿着一条路径尽可能深地探索下去，直到不能再继续深入或者达到目标节点，然后回溯到上一个节点，继续探索其他未被访问的分支路径，直到遍历完所有节点或者找到目标节点为止。

- 例如，在一个树状结构中，从根节点出发，先访问根节点的一个子节点，然后以这个子节点为新的起点，继续访问它的子节点，一直深入到叶子节点。当到达叶子节点无法继续深入时，就回溯到上一层节点，去探索该节点的其他子节点。

- 方法：站在回溯树的一个节点上，你只需要思考 3 个问题：

1、路径：也就是已经做出的选择。

2、选择列表：也就是你当前可以做的选择。

3、结束条件：也就是到达决策树底层，无法再做选择的条件。

- DFS模版(全排列问题cpp)

```cpp
trackbacks(num,used,tracks){
    if(tracks.szie()==used.size()){//选择好了一个答案
        res.push_back(track);
        return;
    }//base case 和递归出口
    for(int i=0;i<nums.size();i++){//选择列表
        if(used(i)){//从选择列表中选择
            continue;
        }//这是最基本的剪枝逻辑
        used[i]=true;
        tracks.push_back(num[i]);//加入路劲
        trackbacks(num,used,tracks);
        used[i]=false;
        tracks.pop_back();//回溯操作
    }
}
```

#### 2.1.1、数独问题

- 9*9的数独填空9*9的9叉树，但是会有很多剪枝

```cpp
class solution{
private:
    bool found=false;
    int m=9,n=9;
public:
    void shudusolution(vector<vector> >board){
        backtrack(board,0);
    }
    backtrack(vector<vector> >&board,int index){
        int i=index/m,j=index%n;//把二维数组变成一维的表示
        if(found)return;//递归出口
        if(index==m*n){
            found=true;
        }
        if(board[i][j]!='.'){
            backtrack(board,index+1);
        }//base case
        for(char ch='1';ch<='9';ch++){
            if(!is_valid(board,i,j,ch)){
                continue;
            }//剪枝
            board[i][j]=ch;
            backtrack(board,index+1);
            if(found){
                return;
            }
            board[i][j]='.';
        }
    }
    bool is_valid(vector<vector<int> >&board,int r,int c,char ch){
        for(int i=0;i<9;i++){
            if(board[r][i]==ch)return false;
            if(board[i][l]==ch)return false;
            if(board[(r/3)*3+i/3][(c/3)*3+i%3]==ch)return false;//每个小方格的每一个数字都相等
        }
        return true;
    }
};
```

#### 2.1.2、n位二进制数的全排列

```cpp
class solution{
private:
 vector<vector<int> >res;
public:
 vector<int>tracks;
 vector<vector<int> >binaryGenarate(int n){
  trackbacks(n,0);
  return res;
 } 
 void trackbacks(int n,int i){
  if(i==n){
   res.push_back(tracks);
   return;
  }
  for(int val:{0,1}){
   tracks.push_back(val);
   trackbacks(n,i+1);
   track.pop_back();
  }
 }
};
```

#### 2.1.3 凑24(或者凑某个数)

- 用堆栈，vector,DFS,两个循环去选择要算的数，再一个循环去把未选择的加入vector中，再循环op(符号)将选择的两个数原运算结果加进去，最后只剩一个看是不是要求的数（相当于把所有数放到一个集合，再取出两个，算出结果，放回那个集合，再重复，又取出两个数）

- 注意：最底层的递归出口和中途回溯的区别，当有一个成功，就一直回溯返回，到递归最底层发现不行，只回溯一层，然后选择另一条路进行走，当所有路都走完了，再返回false;当输入某个值结束的判断方法，用break,判断实数相等的方法

- 现在的问题是，是否存在一种方式使得得到的表达式的结果等于24。这里加减乘除以及括号的运算结果和运算的优先级跟我们平常的定义一致（这里的除法定义是实数除法）。

```cpp
#include <bits/stdc++.h>
using namespace std;
const double MINNUM=1e-6;
bool is_equal(double a,double b){
    return fabs(a-b)<MINNUM;
}//判断
bool DFS(vector<douuble>nums){
    if(nums.size()==1){
        if(is_equal(nums[0],24)){
            return true;
        }
        return false;//最底层的递归出口
    }
    int n=nums.size();
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(i==j)continue;
            vector<double>newNum;
            for(int k=0;k<n;k++){
                if(k==i&&k==j)continue;
                newNum.push_back(nums[k]);
            }
            for(int op=0;op<4;op++){
                if(op==0){
                    nexNum.push_back(nums[i]+nums[j]);
                }else if(op==1){
                    nextNum.push_back(nums[i]-nums[j]);
                }else if(op==2){
                    next.push_back(nums[i]*nums[j]);
                }else{
                    if(nums[j]!=0){
                        newNum.push_back(nums[i]/nums[j]);
                    }
                }
                if(DFS(newNum)){
                    return true;
                }
                newNum.pop_back();//回溯
            }
        }
    }
    return false;
}
int main()
{
    vector<int>a(4);
    while(true){
        for(int i=0;i<4;i++){cin>>a[i];}
        if(a[0]==0&&a[1]==0&&a[2]==0&&a[3]==0)break;
        vector<double>nums(a.begin(),a.end());//vector的复制
        if(DFS(nums)){
            cout<<"YES"<<endl;
        }else{
            cout<<"NO"<<endl;
        }
    }
    return 0;
}
```

#### 2.1.4N皇后问题

- 以行作为DFS的基准，DFS的常见套路;

```cpp
bool is_valid(vector<vector<int>&board,int row,int col){
    int n=board[0].size();
    for(int i=0;i<row;i++){
        if(board[i][col]==1)return false;
    }
    for(int i=row-1,j=col-1;i>=0&&j>=0;i--,j--){
        if(board[i][j]==1)return false;
    }
    for(int i=row-1,j=col+1;i>=0&&j<n;i--,j++){
        if(board[i][j]==1)return false;
    }
    return true;
}
```

- 上面是判断是否有效的函数，即在那一行放入皇后是否可以

```cpp
void backtrace(vector<vector<int> >&board,int row){
    if(row==n){//这里一定是n
        res++;
        return;
    }
    for(int i=0;i<n;i++){
        if(!is_valid(board,row,col)){
            continue;
        }
        board[row][col]=1;
        backtrace(board,row,col);
        board[row][col]=0;
    }
}
```

### 2.2走迷宫问题

#### 走迷宫

#### 矩阵中的最长递增路径

- 描述：给定一个 m x n 整数矩阵 matrix ，找出其中最长递增路径的长度。对于每个单元格，你可以往上，下，左，右四个方向移动。 你不能在对角线方向上移动或移动到边界外（即不允许环绕）。

- 输入：第一行n，m。1<=n,m<=200，接下来是n*m的矩阵

```3 3
9 9 4 
6 6 8 
2 1 1 
```

- 输出:
4
- dfs结合动态规划dp数组

```cpp
#include <bits/stdc++.h>
using namespace
const int N=210;
int dp[N][N];
int n,m;
int G[N][N];
bool vis[N][N];
int dx[4]={1,0,-1,0};
int dy[4]={0,1,0,-1};
int DFS(int x,int y){
    for(int i=0;i<4;i++){
        int new_x=x+dx[i];
        int new_y=x+dy[i];
        if(new_x>=0&&new_x<n&&new_y>=0&&new_y<m&&G[new_x][new_y]>G[x][y]){
            if(vis[new_x][new_y])dp[x][y]=max{dp[x][y],dp[new_x][new_y]+1};
            else{
                dpp[x][y]=max{dp[x][y],DFS(new_x,new_y)}
            }
        }
    }
    vis[x][y]=true;
    return dp[x][y];
}
int main(){
    cin>>n>>;
    for(int i=0=;i<n;i++){
        for(int j=0;j<m;j++){
            cin>>G[i][j];
            dp[i][j]=1;
            vis[i][j]=false;
        }
    }
    int ans;
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            if(vis[i][j])ans=max(ans,dp[i][j]);
            else{
                ans=max(ans,DFS(i,j));
            }
        }
    }
    cout<<ans<<endl;
    return 0;
} 
```

## 2.2.图论

### 2.2.1最大流问题

- 可行流：每条链上的实际流量小于最大流量，每个节点的流入量等于流出量；

- 求最大流：从源点到汇点的最大流量，标号法：先将源点为无穷大流，再找已经标号节点的邻居节点（队列），标上一个节点和正向弧的较小值，将其前驱结点记录下来，重复进行直到最后一个节点，再进行回溯，正向回溯的弧流以最大容量减去后一个节点增加的，反向加上，因为是看的每个弧上的还剩的流量

- 问题：第1行:两个用空格分开的整数M,N(0<M≤200,0<N≤200)。M是方案中的路段数量,N是路线交汇点的数量。其中点1是生产   地,点N是目的地。第2行到第M+1行:每行有三个整数Si,Ei,Ci。 其中Si​和Ei(1≤Si,Ei≤N)指明该路段的两个端点,口罩从Si​单向运往Ei​。Ci(0<Ci≤100000)是这段路线的最大运输量.

- 输出：该方案的最大运输量

- 注意：这种带权的问题一般用邻接矩阵，vector<unordered_set<int> >也要注意
BFS的常见写法

```plaintext
while(!Q.empty()){
    int u=Q.front();
    Q.pop();
    for(int v;v<N;v++){
        if(G[u][v]>0&&!vis[v]){
            ....
            Q.push(v);
        }
    }
}
```

```cpp
#include <bits/stdc++.h>
using namespace std;
int M,N;
vector<vector> >Graph(200,vector<int>(200,0));
int pre[201];
int flow[201];
int s,t;
const int inf=0x3f3f3f3f;
bool BFS(){
    fill(pre,pre+N,-1);
    fill(flow,flow+N,0);
    queue<int>Q;
    Q.push(s);
    flow[s]=inf;
    while(!Q.empty()){
        int u=Q.front();
        Q.pop();
        for(int v=0;v<N;v++){
            if(G[u][v]>0&&pre[v]==-1){
                flow[v]=min(G[u][v],flow[u]);
                pre[v]=u;
                if(v==t)return true;
                Q.push(v);//BFS的常见套路
            }
        }
    }
    return false;
}
int maxFlow(){
    int maxflow=0;
    while(BFS()){
            maxflow+=flow[t];
            int v=t;
            while(v!=s){
                int u=pre[v];
                Graph[u][v]-=node[t];//看的是剩下的
                Graph[v][u]+=ndoe[t];//反向应该与正向一样大
                v=u;
            }
    }
    return maxflow;
}
int main()
{
    cin>>M>>N;
    s=0;
    t=N-1;//从0开始计数
    for(int i=0;i<M;i++){
        int u,v,w;
        cin>>u>>v>>w;
        u--;
        v--;
        G[u][v]=w;
    }
    int ans=maxFlow();
    cout<<ans<<endl;
    return 0;
}
```

### 求最短路径

- 问题描述:给定一个n个点m条边的有向图，图中可能存在重边和自环，所有边权均为正值。
请你求出1号点到其他点的最短距离，如果无法从1号点走到n号点，则输出-1。
- 输入
第一行包含整数n和m。1<=n<=500, 1<=m<=100000
接下m行每行包含三个整数x，y，z，表示存在一条从点x到点y的有向边，边长为z。
- 输出:输出n个整数，表示1号点到n号点的最短距离。

- dijkstra:两次循环，edge{int to,int w};图结构vector<edge>e[N];dis[N]:到源点的距离；vis[N]来标记是否出圈（被选中了，下一次就不能再选），第一次循环是层数，第二层循环有两个，第一个是找那一层最小的路径的节点，第二个循环是找那个节点的的相邻点去进行松弛操作是否成立。

```cpp
#include <bits/stdc++.h>
using namespace std;
const int inf=0x3f3f3f3f;
const int N=100;
int n,m;
struct edge{int to,w;};
int dis[N];
vector<edge> e[N];//一个数组，数组里面是与节点相连节点的vector，vector包含节点和与那个节点的权值 
bool vis[N]={false};
void dijkstra(int start){
 for(int i=0;i<=n;i++){
  dis[i]=inf;  
 }
 dis[start]=0;
 for(int i=0;i<n;i++){
  int u=-1;
  int mind=inf;
  for(int j=1;j<=n;j++){
   if(!vis[j]&&dis[j]<mind){
    u=j;
    mind=dis[j];
   }
  }
  if(u==-1)break;
  vis[u]=true;
  for(int j=0;j<e[u].size();j++){//每个位置的访问结界一定要搞清楚 
   int v=e[u][j].to;
   int w=e[u][j].w;
   if(dis[v]>dis[u]+w){
    dis[v]=dis[u]+w;
   }
  }
 }
}
int main()
{
 cin>>n>>m;
 for(int i=0;i<m;i++){
  int u,v,w;
  cin>>u>>v>>w;
  e[u].push_back({v,w});
 }
 int start=1;
 dijkstra(start);
 for(int i=1;i<=n;i++){
  if(dis[i]!=inf){
   cout<<dis[i]<<" ";
  }else{
   cout<<-1<<" ";
  }
 }
 return 0; 
}
```

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

- prim算法，加点法，任意从一个点出发，加入已经选的点出发的最小边的点，且不会形成环，最后把所有的点都加进来

```cpp
#include <iostream>
#include <vector>
using namespace std;
const int N=510;
int m,n;
int g[N][N];
bool vis[N];
int dist[N];//从已选的i节点出发到剩下没选的最短路劲
int prim(void){
    memset(dist,0x3f,sizeof(dist));
    dist[1]=0;//1节点为参考点1到1的时候为0
    int res=0;
    for(int i=0;i<n;i++){
        int t=-1;
        for(int j=1;j<=n;j++){
            if(!vis[j]&&t==-1||dist[t]<dist[j]){
                t=j;
            }
            if(dist[t]==0x3f3f3f3f)return 0x3f3f3f3f;
            res+=dist[t];
            vis[t]=true;
        }
        for(int j=1;j<=n;j++){
            dist[j]=min(dist[j],g[t][j]);//到达已选部分每个没选节点所需要的最小代价，只需要把新加入的节点比较一下就可以了
        }
    }
    return res;
}
int main()
{
    cin>>n>>m;
    for(int i=0;i<m;i++){
        int u,v,w;
        cin>>u>>v>>w;
        g[u][v]=g[v][u]=min(g[u][v],w);
    }
    int ant=prim();
    cout<<ant<<endl;
    return 0;
}
```

## 动态规划

### 0-1背包问题

- 问题描述：你只有c元，每件商品有价格w[i],价值v[i],从n件商品中选择,使得总价值最大，每件商品最多只能选择一件
- 限制：当V过于大时，动态数组dp开的过于大会超出限制，这时就要用到回溯，DFS

```cpp
int knapsack(int n,int c,vector<int>&w,vector<int>&v){
    vector<vector<int> >dp(n+1,vector<int>(c+1,0));
    for(int i=v[0];i<=V;i++){
        dp[0][i]=w[i];//从0开始计数的初始化
    }
    for(int i=1;i<=n;i++){//这里一定要从1开始，如果是从0开始的第一个数，一定要初始化，但是这里还是从1开始
        for(int j=1;j<=c;j++){
            dp[i][j]=dp[i-1][j];//不选择
            if(j>=w[i]){
                dp[i][j]=max(dp[i-1][j],dp[i-1][j-w[i]]+v[i]);
            }
        }
    }
    return dp[n][c];
}
```

```cpp
backtrace(0,0,0);
void backtrace(int index,int currentW,int currentV){
    if(currentW>max_ans)
    max_ans=currentV;
    for(int i=index;i<=N;i++){
        if(currentV+v[i]<V){
            backtrace(inedx+1,currentW+w[i],currentV+v[i]);
        }
    }
}
```

### 多重背包问题

- 问题简述：然而物品是有限的，第i个物品的价格为w[i]、参考的价值为v[i]，能买到的最大物品数量为s[i]，你需要以你的经验来购买获得最大的价值。
- 输入：第一行2个数n(n<=500)，m(m<=6000)，其中n代表物品的种数，m表示你的经费。接下来n行，每行3个数，w[i] (w[i]<=100)、v[i] (v[i]<=1000)、s[i] (s[i]<=10)。

- 上代码

```cpp
int mulknapsack(int n,int k,vector<int>&w,vector<int>&v,vector<int>&s){
    vector<vector<int> >dp(n+1,vector<int>(c+1,0));//前i件商品背包容量为j；
    for(int i=1;i<=n;i++){
        for(int j=1;j<=c;j++){
            dp[i][j]=dp[i][j-1];//第i件都不选
            for(int k=1;k<=s[i]&&k*w[i]<=j;k++){//第i件选k个
                dp[i][j]=max(dp[i-1][j],dp[i-1][j-k*w[i]]+k*v[i]);
            }
        }
    }
    return dp[n][c];
}
```

### 最长递增子序列

- 从数组中找子序列使的最长

```cpp
vector<int>dp(N,1);//子问题:以i为结尾的最长子序列
for(int i=0;i<N;i++){
    for(int j=0;j<i;j++){
        if(Ti[i]>Ti[j]){
            dp[i]=max(dp[i],dp[j]+1);//看此时已有的长度和前面某一个的长度加一谁大
        }
    }
}
```
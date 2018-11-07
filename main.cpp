#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

class vertice{
public:
    //the vertice ID
    int id;
    //the degree of vertice
    int d;
    //the structral diversity of vertices
    int ds;

    vertice()
    {
        id = 0;
        d = 0;
        ds = 0;
    }
};

class Node{
public:
    int v, id;
};

//stores the info of vertices
vertice * vert = NULL;
//vcnt -- the number of vertices
int vcnt = 430000;
//stores the info of undirected edges -- ID
vector<int> vec[430000];
//stores the info of directed edges -- ID
vector<int> newVec[430000];
//the integer
int t = 2;
//top-k
int k = 100;
//stores the top-k results
int *r = NULL;
//stores the number of vertices that has been found
int r_cnt = 0;
//computing the intersection of N+(u) and N+(v).
bool *b = NULL;
int maxd = 0;

bool cmp3(Node a, Node b)
{
    return a.v < b.v;
}

class UF{
public:
    int cnt;
    int n_cnt;
    int * parent;
    int * s;
    Node * node;

    UF()
    {
        cnt = 0;
        n_cnt = 0;
        parent = NULL;
        s = NULL;
        node = NULL;
    }
    UF(int u, int n)
    {
        cnt = n;
        n_cnt = n;
        parent = (int *)malloc(sizeof(int) * n);
        s = (int *)malloc(sizeof(int) * n);
        node = (Node *)malloc(sizeof(Node) * n);
        for (int i = 0; i < n; i ++)
        {
            parent[i] = i;
            s[i] = 1;
            node[i].v = vec[u][i];
            node[i].id = i;
        }
    }
    ~UF()
    {

        delete[] parent;
        delete[] s;
    }
    int find(int x, int tmp)
    {
        if (tmp == 0)
        {
            for (int i = 0; i < n_cnt; i ++)
            {
                if (node[i].v == x)
                {
                    x = node[i].id;
                    break;
                }
            }
        }
        if (x != parent[x])
            parent[x] = find(parent[x], tmp + 1);
        return parent[x];
    }
    void uion(int root1, int root2)
    {
        root1 = this->find(root1, 0);
        root2 = this->find(root2, 0);
        if (root1 == root2)
            return;
        else if (s[root1] < s[root2])
        {
            s[root2] += s[root1];
            parent[root1] = root2;
        }
        else
        {
            s[root1] += s[root2];
            parent[root2] = root1;
        }
        cnt --;
    }
};

UF **uf = NULL;

bool cmp(vertice a, vertice b)
{
    //degree decreasing
    if (a.d == b.d)
        return a.id < b.id;
    return a.d > b.d;
}

bool cmp2(int a, int b)
{
    if (vert[a].ds == vert[b].ds)
        return vert[a].id < vert[b].id;
    return vert[a].ds > vert[b].ds;
}

void insert(int u, int v, int w)
{
    int ui = -1;
    for (int i = 0; i < vcnt; i ++)
    {
        if (vert[i].id == u)
        {
            ui = i;
            break;
        }
    }
    int s1 = uf[u]->find(v, 0);
    int s2 = uf[u]->find(w, 0);
    int s1Cnt = uf[u]->s[s1];
    int s2Cnt = uf[u]->s[s2];
    if (s1 != s2)
    {
        if (max(s1Cnt, s2Cnt) < t && s1Cnt + s2Cnt >= t)
            vert[ui].ds ++;
        else if (s1Cnt >= t && s2Cnt >= t)
            vert[ui].ds --;
        uf[u]->uion(s1, s2);
    }
}

void Updata(int u)
{
    if (r_cnt < k)
    {
        r[r_cnt] = u;
        r_cnt ++;
    }
    else
    {
        if (vert[ r[r_cnt - 1]].ds < vert[u].ds)
            r[r_cnt - 1] = u;
        else if (vert[ r[r_cnt - 1]].ds == vert[u].ds && vert[ r[r_cnt - 1]].id < vert[u].id)
            r[r_cnt - 1] = u;
        else
            return;
    }
    sort(r, r + r_cnt, cmp2);
}

int main()
{
    //u, v, w -- the ID number of vertice
    int u, v, w;
    FILE * fp = NULL;

    fp = fopen("../dblp.txt", "r");
    if (fp == NULL)
    {
        printf("fail!\n");
        return 0;
    }

    r = new int[k];
    vert = new vertice[vcnt];
    memset(r, 0, sizeof(r));
    memset(vert, 0, sizeof(vert));

    vcnt = 0;
    fscanf(fp, "%d%d", &u, &v);
    vcnt = max(vcnt, max(u + 1, v + 1));
    while(!feof(fp))
    {
        vec[u].push_back(v);
        vec[v].push_back(u);
        vert[u].d ++;
        vert[v].d ++;
        fscanf(fp, "%d%d", &u, &v);
        vcnt = max(vcnt, max(u + 1, v + 1));
    }
    vec[u].push_back(v);
    vec[v].push_back(u);
    vert[u].d ++;
    vert[v].d ++;

    uf = new UF*[vcnt];
    for (int i = 0; i < vcnt; i ++)
    {
        vert[i].id = i;
        if (t == 1)
            vert[i].ds = vert[i].d;
        else
            vert[i].ds = 0;
        //Construct the oriented graph G+ = (V,E+) of G;
        for (int j = 0; j < vec[i].size(); j ++)
        {
            if (vert[i].d > vert[vec[i][j]].d)
                newVec[i].push_back(vec[i][j]);
            else if (vert[i].d == vert[vec[i][j]].d && i > vec[i][j])
                newVec[i].push_back(vec[i][j]);
        }
        //initialize a disjoint-set data structure with elements N(v)
        uf[i] = new UF(i, vec[i].size());
    }
    sort(vert, vert + vcnt, cmp);
    //initialize a Boolean array B of size n, all entries to be false
    b = new bool[vcnt];
    for (int i = 0; i < vcnt; i ++)
        b[i] = false;

    for (int i = 0; i < vcnt; i ++)
    {
        cout << "i = " << i << endl;
        u = vert[i].id;
        if (r_cnt == k && vert[ r[r_cnt - 1] ].ds >= vert[i].d / t)
            break;
        for (int j = 0; j < newVec[u].size(); j ++)
        {
            v = newVec[u][j];
            b[v] = true;
        }
        for (int j = 0; j < newVec[u].size(); j ++)
        {
            v = newVec[u][j];
            for (int p = 0; p < newVec[v].size(); p ++)
            {
                w = newVec[v][p];
                if (b[w])
                {
                    insert(u, v, w);
                    insert(v, u, w);
                    insert(w, u, v);
                }
            }
        }
        for (int j = 0; j < newVec[u].size(); j ++)
        {
            v = newVec[u][j];
            b[v] = false;
        }
        //update R with u and its score Ds(u)
        Updata(i);
    }
    for (int i = 0; i < min(k, vcnt); i ++)
    {
        printf("vertice %d: %d\n", i, vert[ r[i] ].id);
    }
    return 0;
}
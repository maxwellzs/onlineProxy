#include <iostream>
#include <stack>
#include <cstring>
#include <vector>

__attribute__((constructor)) void desync_iostream() {
    std::iostream::sync_with_stdio(false);
    std::cin.tie(0);
}

#ifdef TEST
const int N = 200;
#else 
const int N = 3e5 + 10;
#endif

struct Edge {
    long long weight;
    int next,to;
} v[N * 2];
int head[N] = {0}, cnt = 1;

#define add_edge(f, t, w) { \
    v[++cnt].to = t; \
    v[cnt].next = head[f]; \
    v[cnt].weight = w; \
    head[f] = cnt; \
}

long long dis[N] = {0};
int trace[N] = {0};
long long traced[N] = {0};
int vis[N] = {0}, curVis = 0;
bool bin[N] = {false};

long long dfs(int st) {
    vis[st] = curVis;
    for(int i = head[st];i ;i = v[i].next) {
        Edge e = v[i];
        if(vis[e.to] == curVis || bin[e.to]) continue;
        dis[e.to] = dfs(e.to);
        if(dis[st] < dis[e.to] + e.weight) dis[st] = e.weight + dis[e.to];
    }
    return dis[st];
}

long long bfs(int st) {
    curVis++;
    long long ans = st;
    trace[st] = 0;
    traced[st] = 0;
    dis[st] = 0;
    vis[st] = curVis;
    std::stack<int> call;
    call.push(st);
    while(!call.empty()) {
        int l = call.top();
        call.pop();
        for(int i = head[l];i;i = v[i].next) {
            Edge e = v[i];
            if(vis[e.to] == curVis) continue;
            trace[e.to] = l;
            traced[e.to] = e.weight;
            dis[e.to] = dis[l] + e.weight;
            if(dis[e.to] > dis[ans]) ans = e.to;
            vis[e.to] = curVis;
            call.push(e.to);
        }
    }
    return ans;
}

long long disLeft[N] = {0}, disRight[N] = {0}, mid[N] = {0};

#define max(a,b) (a > b ? a : b)
#define min(a,b) (a > b ? b : a)


#define work(d,beg,end,res) \
{ \
    long long ans = max(disLeft[d[beg]],disRight[d[end]]); \
    for(int i = beg + 1;i != end; i ++) { \
        ans = max(ans, mid[d[i]]); \
    } \
    res = ans; \
}

int main(int argc,char ** argv) {
    int n,s;
    std::cin >> n >> s;
    for(int i = 0;i < n-1;i ++) {
        int from,to;
        long long weight;
        std::cin >> from >> to >> weight;
        add_edge(from,to,weight);
        add_edge(to,from,weight);
    }
    // 两次bfs求直径
    int p = bfs(1);
    int q = bfs(p);
    // std::cout << dis[q] << std::endl;
    int cur = q;
    long long discur = 0;
    // 还原直径边权，以及点
    std::vector<int> d;
    while(cur) {
        disLeft[cur] = discur;
        disRight[cur] = dis[q] - discur;
        discur += traced[cur];
        d.push_back(cur);
        bin[cur] = true;
        cur = trace[cur];
    }
    memset(dis,0,sizeof(long long) * N);
    for(auto i : d) {
        curVis ++;
        dis[i] = 0;
        // dfs求不在直径上的点到达直径的最大距离
        mid[i] = dfs(i);
    }
    int beg = 0,end = 0;
    long long sum = 0,ans = 0x7fffffffffffff;
    bool hasAns = false;
    // 尺取法求最大值最小的区间
    while(true) {
        while(end < d.size() - 1 && sum < s) {
            sum += traced[d[end++]];
            if(sum <= s) {
                long long curAns;
                work(d,beg,end,curAns);
                hasAns = true;
                ans = min(ans,curAns);
            }
        }
        if(sum <= s) {
            long long curAns;
            work(d,beg,end,curAns);
            ans = min(ans,curAns);
            hasAns = true;
            break;
        }
        while(beg < end) {
            sum -= traced[d[beg++]];
            if(sum <= s && beg < end) {
                long long curAns;
                work(d,beg,end,curAns);
                hasAns = true;
                ans = min(ans,curAns);
            }
        }
        if(beg == d.size() - 1) break;
    }
    std::cout << ans << std::endl;
    return 0;
}
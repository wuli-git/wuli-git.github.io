#include <iostream>
#include <vector>
#include <cstring>
using namespace std;
int mincount(const string& S,const vector<vector<string> >& pakeage){
	int s=S.size();
	int n=pakeage.size();
	vector<vector<int> >dp(n+1,vector<int>(s+1),INT_MAX);//还有N个包字符串前s个的时候的最小次数;
	dp[0][0]=0;
	for(int i=1;i<=n;i++){
		for(int j=0;j<=s;j++ ){
			dp[i][j]=dp[i-1][j];
		}
		for(const string& str:pakeage[i]){
			int len=str.length();
			for(int j=len;j<=s;j++){
				if(dp[i-1][j-len]!=INT_MAX&&s.substr(j-len,len)==str){
					dp[i][j]=min(dp[i][j],dp[i-1][j-len]+1);
				}
			}
		}
	} 
	return dp[n][s]==INT_MAX?-1:dp[n][s];
}
int main()
{
	string S;
	cin>>S;
	int N;
	cin>>N;
	vector<vector<string> >pakeage(N);
	for(int i=0;i<N;i++){
		int A_i;
		cin>>A_i;
		pakeage[i].resize(A_i);
		for(int j=0;j<A_i;j++){
			cin>>pakeage[i][j];
		} 
	}
	int result=mincount(S,pakeage);
	cout<<result<<endl;
	return 0;
 } 

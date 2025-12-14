#pragma once
#include<math.h>
#include<vector>
#include<string>
#include<complex>
#include<stdexcept>
using std::exp;
using std::string;
using std::vector;
using std::complex;
using std::runtime_error;
complex<long double> j(0,1);
long double ld_eps=1e-9;
constexpr long double pi=3.14159265358979323846264338327950288419716939937;
inline unsigned int fast_bit_reverse32(unsigned int x)
{
	x=((x>>1)&0x55555555)|((x&0x55555555)<<1);
	x=((x>>2)&0x33333333)|((x&0x33333333)<<2);
	x=((x>>4)&0x0f0f0f0f)|((x&0x0f0f0f0f)<<4);
	x=((x>>8)&0x00ff00ff)|((x&0x00ff00ff)<<8);
	x=(x>>16)|(x<<16);
	return x;
}
inline unsigned int bit_reverse(unsigned int x,unsigned int log2n)
{
	return fast_bit_reverse32(x)>>(32-log2n);
}
inline vector<complex<long double>> fft(vector<complex<long double>> data)
{
	unsigned long long n=data.size();
	if(n&(n-1))
		throw runtime_error("size of input is not exponent of 2");
	unsigned int log2n=63-__builtin_clzll(n);
	for(unsigned int i=0;i<n;i++)
	{
		unsigned int j=bit_reverse(i,log2n);
		if(j>i)
			swap(data[i],data[j]);
	}
	for(unsigned int s=1;s<=log2n;s++)
	{
		unsigned int m=1<<s,m2=m>>1;
		complex<long double> wm=exp(-2.0l*pi*j/(long double)m);
		for(unsigned int k=0;k<n;k+=m)
		{
			complex<long double> w=1;
			for(unsigned int j=0;j<m2;j++)
			{
				complex<long double> t=w*data[k+j+m2];
				complex<long double> u=data[k+j];
				data[k+j]=u+t;
				data[k+j+m2]=u-t;
				w*=wm;
			}
		}
	}
	for(size_t i=0;i<n;i++)
		data[i]/=(long double)n;
	return data;
}
inline vector<complex<long double>> ifft(vector<complex<long double>> data)
{
	unsigned long long n=data.size();
	if(n&(n-1))
		throw runtime_error("size of input is not exponent of 2");
	unsigned int log2n=63-__builtin_clzll(n);
	for(unsigned int i=0;i<n;i++)
	{
		unsigned int j=bit_reverse(i,log2n);
		if(j>i)
			swap(data[i],data[j]);
	}
	for(unsigned int s=1;s<=log2n;s++)
	{
		unsigned int m=1<<s,m2=m>>1;
		complex<long double> wm=exp(2.0l*pi*j/(long double)m);
		for(unsigned int k=0;k<n;k+=m)
		{
			complex<long double> w=1;
			for(unsigned int j=0;j<m2;j++)
			{
				complex<long double> t=w*data[k+j+m2];
				complex<long double> u=data[k+j];
				data[k+j]=u+t;
				data[k+j+m2]=u-t;
				w*=wm;
			}
		}
	}
	return data;
}

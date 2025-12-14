#pragma once
#include<complex>
#include<vector>
#include"fft.h"
using std::min;
using std::max;
using std::runtime_error;
class FFT_filter
{
	private:
		double sr;
		size_t fft_size;
	public:
		FFT_filter(double sr,size_t size=1024)
		{
			this->sr=sr;
			this->fft_size=size;
		}
		struct peak
		{
			double center;
			double bandwidth;
			double gain;
			peak(double cf,double bw,double g)
			{
				center=cf;
				bandwidth=bw;
				gain=g;
			}
		};
		struct peak_list
		{
			std::vector<std::vector<peak>> peaks;
		};
	private:
		inline double calculate_peak_gain(double freq,const peak &p)
		{
			if(p.bandwidth<=0)
				return 1;
			//use the gaussian shape
			double dist=std::abs(freq-p.center)/p.bandwidth*2;
			double attenuation=std::exp(-dist*dist);
			return 1+(p.gain-1)*attenuation;
		}
		inline double calculate_background_attenuation(double freq,const std::vector<peak> &peaks)
		{
			double att=1.0;
			double min_dist=1e9;
			for(const peak &p:peaks)
				min_dist=min(min_dist,std::abs(freq-p.center)/p.bandwidth);
			if(min_dist>2)
				att=std::exp((2-min_dist)*0.8);
			return att;
		}
		std::vector<double> apply_filter(const std::vector<double> &input,const std::vector<std::complex<long double>> &response)
		{
			size_t n=input.size();
			//time domain
			std::vector<std::complex<long double>> td(fft_size,0.0);
			for(size_t i=0;i<n;i++)
				td[i]=input[i];
			//input frequency domain
			vector<std::complex<long double>> input_fd=fft(td);
			//frequency domain multiplication
			for(size_t i=0;i<fft_size;i++)
				input_fd[i]*=response[i];
			std::vector<std::complex<long double>> output=ifft(input_fd);
			std::vector<double> result(n);
			for(size_t i=0;i<n;i++)
				result[i]=std::real(output[i]);
			return result;
		}
		std::vector<double> create_hanning_window(size_t size)
		{
			std::vector<double> window(size);
			for(size_t i=0;i<size;i++)
				window[i]=0.5-0.5*std::cos(2*pi*i/(size-1));
			return window;
		}
	public:
		std::vector<std::complex<long double>> create_response(const std::vector<peak> &peaks)
		{
			std::vector<std::complex<long double>> response(fft_size,1);
			double freq,mag;
			for(size_t i=0;i<(fft_size>>1);i++)
			{
				freq=i*sr/fft_size;
				mag=1.0;
				for(const peak &p:peaks)
					mag*=calculate_peak_gain(freq,p);
				response[i]=mag*calculate_background_attenuation(freq,peaks);
				if(i>0)
					response[fft_size-i]=mag;
			}
			return response;
		}
		std::vector<double> filter(const std::vector<double> &input,const std::vector<peak> &peaks)
		{
			std::vector<std::complex<long double>> response=create_response(peaks);
			return apply_filter(input,response);
		}
		std::vector<double> filter_blockwise(const std::vector<double> &input,const std::vector<peak> &peaks,size_t len,size_t blocksize=512)
		{
			std::vector<std::complex<long double>> fr=create_response(peaks);
			std::vector<double> output(len,0.0);
			size_t overlap=blocksize>>2;
			std::vector<double> window=create_hanning_window(blocksize);
			for(size_t start=0;start<len;start+=blocksize-overlap)
			{
				size_t end=std::min(start+blocksize,len);
				size_t current_size=end-start;
				std::vector<double> block(blocksize,0.0);
				for(size_t i=0;i<current_size;i++)
					block[i]=input[start+i]*window[i];
				std::vector<double> filtered_block=apply_filter(block,fr);
				for(size_t i=0;i<current_size&&start+i<len;i++)
					output[start+i]+=filtered_block[i]*window[i];
			}
			return output; 
		}
		std::vector<double> filter_blockwise(const std::vector<double> &input,const peak_list &pl,size_t len,size_t blocksize=512)
		{
			std::vector<double> output(len,0.0);
			size_t overlap=blocksize>>2;
			std::vector<double> window=create_hanning_window(blocksize);
			for(size_t start=0;start<len;start+=blocksize-overlap)
			{
				double time_ratio=(double)start/len;
				int peak_index1=(int)(time_ratio*(pl.peaks.size()-1));
				int peak_index2=min(peak_index1+1,(int)pl.peaks.size()-1);
				double segment_length=(double)len/(pl.peaks.size()-1);
				double local_ratio=(start-peak_index1*segment_length)/segment_length;
				local_ratio=max(0.0,min(1.0,local_ratio));
				std::vector<peak> interp_peaks;
				if(pl.peaks[peak_index1].size()!=pl.peaks[peak_index2].size())
					throw runtime_error("resonant peak list sizes not equal");
				for(size_t i=0;i<pl.peaks[peak_index1].size();i++)
				{
					const peak &p1=pl.peaks[peak_index1][i];
					const peak &p2=pl.peaks[peak_index2][i];
					double center=p1.center*pow(p2.center/p1.center,local_ratio);
					double bandwidth=p1.bandwidth+(p2.bandwidth-p1.bandwidth)*local_ratio;
					double gain=p1.gain+(p2.gain-p1.gain)*local_ratio;
					interp_peaks.push_back(peak(center,bandwidth,gain));
				}
				std::vector<std::complex<long double>> fr=create_response(interp_peaks);
				size_t end=std::min(start+blocksize,len);
				size_t current_size=end-start;
				std::vector<double> block(blocksize,0.0);
				for(size_t i=0;i<current_size;i++)
					block[i]=input[start+i]*window[i];
				std::vector<double> filtered_block=apply_filter(block,fr);
				for(size_t i=0;i<current_size&&start+i<len;i++)
					output[start+i]+=filtered_block[i]*window[i];
			}
			return output; 
		}
};
//lowpass filter with cutoff 4kHz to eliminate high-frequency noise 
std::vector<double> lowpass(const std::vector<double>& input)
{
	const int num_taps=51;
//	const double sr=48000.0;
//	const double cutoff=4000.0;
	const double coefficients[]={0,-2.55795e-020,-0.000108551,-0.000439359,-0.000936173,-0.00131439,-0.00113361,1.17618e-018,0.00216985,0.00493991,0.00732159,0.00798948,0.00572918,-3.44764e-018,-0.00857821,-0.0180179,-0.025179,-0.0264248,-0.0185893,5.60869e-018,0.0287522,0.064566,0.102239,0.135483,0.158312,0.16644,0.158312,0.135483,0.102239,0.064566,0.0287522,5.60869e-018,-0.0185893,-0.0264248,-0.025179,-0.0180179,-0.00857821,-3.44764e-018,0.00572918,0.00798948,0.00732159,0.00493991,0.00216985,1.17618e-018,-0.00113361,-0.00131439,-0.000936173,-0.000439359,-0.000108551,-2.55795e-020,0};
	std::vector<double> output(input.size(),0.0);
	std::vector<double> buffer(num_taps,0.0);
	int buffer_index=0;
	double sum=0.0;
	for(size_t i=0;i<input.size();i++)
	{
		sum=0.0;
		buffer[buffer_index]=input[i];
		for(int j=0;j<num_taps;j++)
			sum+=coefficients[j]*buffer[(buffer_index+num_taps-j)%num_taps];
		output[i]=sum;
		buffer_index=(buffer_index+1)%num_taps;
	}
	return output;
}

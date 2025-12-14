#pragma once 
#include<map>
#include<random>
#include<algorithm>
#include"FFT_filter.h"
using std::map;
using std::max;
using std::find;
using std::pair;
using std::fmod;
using std::string;
using std::make_pair;
using std::to_string;
using std::mt19937_64;
using std::random_device;
using std::runtime_error;
using std::uniform_real_distribution;
vector<double> excitement;
inline double decibels(double n)
{
	return pow(10,n/20);
}
inline double to_decibels(double n)
{
	return log10(n)*20;
}
//these are the resonant peak parameters for synthesis
//may be changed for better sounds
static map<string,vector<FFT_filter::peak>> res_peak_mapper=
{
	//single finals
	{"a",{{730,80,decibels(20)},{1090,100,decibels(18)},{2440,150,decibels(15)}}},
	{"o",{{530,90,decibels(24)},{670,100,decibels(20)},{3310,120,decibels(11)}}},
	{"e",{{540,100,decibels(23)},{1040,100,decibels(20)},{3170,120,decibels(9)}}},
	{"i",{{290,100,decibels(19)},{2360,100,decibels(13)},{3570,120,decibels(15)}}},
	{"u",{{380,80,decibels(21)},{440,100,decibels(17)},{3660,120,decibels(10)}}},
	//u in yu
	{"v",{{290,60,decibels(19)},{2160,100,decibels(14)},{3460,120,decibels(13)}}},
	//final variants
	//e hat, like in ei
	{"ee",{{480,80,decibels(23)},{2240,100,decibels(19)},{3470,120,decibels(16)}}},
	//i in zi
	{"ii",{{380,80,decibels(19)},{1380,100,decibels(13)},{3020,120,decibels(10)}}},
	//i in ri
	{"ir",{{390,80,decibels(21)},{1820,100,decibels(17)},{2600,120,decibels(13)}}},
	{"er",{{540,80,decibels(22)},{1600,100,decibels(19)},{3270,120,decibels(11)}}},
	//voiced initials
	{"r",{{340,80,decibels(19)},{1620,100,decibels(15)},{1610,120,decibels(12)}}},
	{"m",{{280,80,decibels(19)},{1180,100,decibels(12)},{2240,120,decibels(11)}}},
	{"n",{{320,80,decibels(19)},{1380,100,decibels(11)},{2680,120,decibels(11)}}},
	{"l",{{310,80,decibels(18)},{1500,100,decibels(14)},{2520,120,decibels(10)}}},
	//y in yi
	{"y",{{280,80,decibels(18)},{2200,100,decibels(14)},{3400,120,decibels(15)}}},
	//y in yu
	{"yy",{{290,80,decibels(19)},{2190,100,decibels(16)},{3360,120,decibels(13)}}},
	{"w",{{390,80,decibels(19)},{670,100,decibels(15)}}},
	//suffix initials
	{"-n",{{280,70,decibels(22)},{1480,100,decibels(12)},{2830,120,decibels(8)}}},
	{"-ng",{{320,80,decibels(20)},{1200,100,decibels(11)},{2880,120,decibels(6)}}},
	//fricative initials
	{"s",{{5000,800,decibels(25)},{6500,1000,decibels(20)}}},
	{"sh",{{2200,600,decibels(23)},{3200,800,decibels(22)},{4500,1000,decibels(18)}}},
	{"x",{{3500,700,decibels(22)},{4800,900,decibels(20)}}},
	{"f",{{200,500,decibels(15)},{6300,800,decibels(12)}}},
	{"h",{{1000,800,decibels(10)},{2000,1000,decibels(8)}}},
	//affricate initials
	{"z",{{4000,700,decibels(16)},{5800,1000,decibels(14)}}},
	{"c",{{2500,600,decibels(20)},{3800,800,decibels(18)},{5000,1000,decibels(15)}}},
	{"zh",{{1800,600,decibels(17)},{2800,800,decibels(15)}}},
	{"ch",{{1600,500,decibels(22)},{2400,700,decibels(20)},{4200,900,decibels(17)}}},
	{"j",{{2800,700,decibels(16)},{4200,900,decibels(14)}}},
	{"q",{{1800,400,decibels(18)},{2800,600,decibels(21)},{4200,800,decibels(17)},{5200,1000,decibels(14)}}},
	//plosive initials
	{"b",{{250,400,decibels(12)},{1200,600,decibels(10)}}},
	{"p",{{800,400,decibels(15)},{1250,600,decibels(13)},{2500,800,decibels(8)}}},
	{"d",{{1800,500,decibels(14)},{3000,800,decibels(12)}}},
	{"t",{{1800,500,decibels(17)},{3000,800,decibels(15)},{4000,1000,decibels(10)}}},
	{"g",{{1500,500,decibels(13)},{2500,700,decibels(11)}}},
	{"k",{{1500,500,decibels(16)},{2500,700,decibels(14)},{3500,900,decibels(9)}}},
	//empty initial
	{"",{{2000,8000,decibels(-10)}}},
};
//here u(as in yu) is replaced with v to avoid charset issues
static map<string,vector<string>> multi_final_mapper=
{
	//special ones: splitted result does not match original spelling
	{"iu",{"i","o","u"}},
	{"ui",{"u","ee","i"}},
	{"un",{"u","e","-n"}},
	{"ie",{"i","ee"}},
	{"ue",{"v","ee"}},
	{"ve",{"v","ee"}},
	{"ou",{"e","u"}},
	//ai, ao, ia, ua, uo, ei, iao, uai
	{"ai",{"a","i"}},
	{"ao",{"a","o"}},
	{"ia",{"i","a"}},
	{"ua",{"u","a"}},
	{"uo",{"u","o"}},
	{"ei",{"e","i"}},
	{"iao",{"i","a","o"}},
	{"uai",{"u","a","i"}},
	//an, ian, uan, van, en, in, vn, ang, iang, uang, eng, ing, ong, iong
	{"an",{"a","-n"}},
	{"ian",{"i","a","-n"}},
	{"uan",{"u","a","-n"}},
	{"van",{"v","a","-n"}},
	{"en",{"e","-n"}},
	{"in",{"i","-n"}},
	{"vn",{"v","-n"}},
	{"ang",{"a","-ng"}},
	{"iang",{"i","a","-ng"}},
	{"uang",{"u","a","-ng"}},
	{"eng",{"e","-ng"}},
	{"ing",{"i","-ng"}},
	{"ong",{"o","-ng"}},
	{"iong",{"i","o","-ng"}},
};
//use the generated excitement data and resonant peak filtering to generate a voice
//voice means a single initial or final
//multi-finals are also accepted
//for the empty initial($(name)=""), a resonant peak with a very large width and very negative gain is used
//so that no sound is generated
short* generate_voice(string name,int len,bool lp_filter)
{
	static FFT_filter filt(48000,1024);
	vector<double> e;
	if(res_peak_mapper.find(name)!=res_peak_mapper.end())
		e=(lp_filter?lowpass(filt.filter_blockwise(excitement,res_peak_mapper[name],len)):filt.filter_blockwise(excitement,res_peak_mapper[name],len));
	else
	{
		vector<vector<FFT_filter::peak>> peaks_list;
		vector<string> vowel_list=multi_final_mapper[name];
		peaks_list.resize(vowel_list.size());
		for(size_t i=0;i<vowel_list.size();i++)
			peaks_list[i]=res_peak_mapper[vowel_list[i]];
		e=(lp_filter?lowpass(filt.filter_blockwise(excitement,FFT_filter::peak_list{peaks_list},len)):filt.filter_blockwise(excitement,FFT_filter::peak_list{peaks_list},len));
	}
	//AGC
	double max_amplitude=0.0;
	for(int i=0;i<len;i++)
		max_amplitude=max(max_amplitude,double(abs(e[i])));
	short *output=new short[len];
	for(int i=0;i<len;i++)
		output[i]=e[i]*(32700/max_amplitude);//leave 67 LSBs to prevent clipping noise
	return output;
}
typedef struct
{
	vector<double> frequency_points;
	size_t length;
} tone;
class RLS_table
{
	private:
		vector<double> table;
		static const size_t size=1048576;
	public:
		RLS_table()
		{
			table.resize(size);
			for(size_t i=0;i<size;i++)
			{
				double phase=double(i)/size;
				table[i]=(phase<0.5?(0.5*(1.0-cos(4.0*pi*phase))-0.25)*1600:(cos(2.0*pi*(phase-0.5))-0.25)*1600);
			}
		}
		double lookup(double phase) const
		{
			phase=fmod(phase,1.0);
			if(phase<0)
				phase+=1.0;
			double pos=phase*size;
			size_t idx1=static_cast<size_t>(pos);
//			size_t idx2=(idx1+1)%size;
//			return table[idx1]*(1.0-pos+idx1)+table[idx2]*(pos-idx1);
			//it seems that the table is precise enough that there is no need to interpolate
			return table[idx1];
		}
};
//note that initial_phase[0] is the phase of DC offset which is not used
//it is just left there to prevent confusion
//type indicates source type:
//0 harmonic source
//1 RLS source
//(in this mode initial_phase is ignored and the function will return the input as-is)
vector<double> generate_tone_excitement(tone t,vector<double> initial_phase,int type)
{
	static RLS_table rt;
	if(t.frequency_points.size()<2)
		throw runtime_error("less than 2 frequency points");
	if(t.length==0)
		return {};
	switch(type)
	{
		//harmonic source(multiple sine waves added together)
		//sounds unnatural and is slow to generate
		//not recommended
		case 0:
		{
			double c=2*pi/48000;
			int len=t.length;
			double l=double(len)/(t.frequency_points.size()-1);
			excitement.assign(len,0.0);
			vector<double> phase;
			phase.resize(50);
			for(int i=1;i<50;i++)
			{
				phase[i]=initial_phase[i];
				for(int j=0;j<len-1;j++)
				{
					excitement[j]+=sin(phase[i])*364;
					//use linear interpolation
					phase[i]+=c*i*(t.frequency_points[int(j/l)]*(l-fmod(j,l))+t.frequency_points[int(j/l)+1]*fmod(j,l))/l;
					phase[i]=fmod(phase[i],2*pi);
				}
				excitement[len-1]+=sin(phase[i])*364;
				phase[i]+=c*i*t.frequency_points[t.frequency_points.size()-1];
				phase[i]=fmod(phase[i],2*pi);
			}
			return phase;
		}
		//RLS wave source
		//sounds much more natural than harmonic source
		//uses precached table to generate, fast
		case 1:
		{
			excitement.assign(t.length,0.0);
			double phase=0;
			double l=double(t.length)/(t.frequency_points.size()-1);
			for(size_t i=0;i<t.length;i++)
			{
				double base_freq=(t.frequency_points[int(i/l)]*(l-fmod(i,l))+t.frequency_points[int(i/l)+1]*fmod(i,l))/l;
				phase+=base_freq/48000;
				if(phase>=1)
					phase-=1;
				//1600 is a pretty random value, AGC will deal with it though
				excitement[i]=rt.lookup(phase);
			}
			return initial_phase;
		}
		//throw error if type is undefined
		default:
			throw runtime_error("unknown excitement source type");
	}
}
double high_freq,low_freq;
//set the highest and lowest frequency of the voice
//highest corresponds to 5 in $(tn())
//lowest corresponds to 1
void set_high_low_freq(double hf,double lf)
{
	high_freq=hf;
	low_freq=lf;
}
//convert tone numeral to frequency
inline double tn(double number)
{
	return pow(high_freq/low_freq,(number-1)/4)*low_freq;
}
//list version
template<typename... Args>
vector<double> tn_list(Args... args)
{
	static_assert(sizeof...(args)>0,"tn_list requires at least one argument");
	return {tn(args)...};
}
//real random number, much better than $(rand())
double rrand()
{
	static random_device rd;
	static mt19937_64 m(rd());
	static uniform_real_distribution<double> dis(-32768,32767);
	return dis(m);
}
//white noise source for fricatives
void generate_noise_excitement(int len)
{
	excitement.assign(len,0.0);
	for(int i=0;i<len;i++)
		excitement[i]=rrand();
}
//pulse noise+white noise
//for affricates
void generate_affricate_excitement(int len,bool aspirated)
{
	excitement.assign(len,0.0);
	int closure_duration=len*0.08;
	int burst_duration=(aspirated?len*0.04:len*0.08);
	int aspiration_duration=(aspirated?len*0.88:len*0.84);
	int aspiration_start=closure_duration+burst_duration;
	for(int i=closure_duration;i<aspiration_start;i++)
		excitement[i]=rrand()*exp(-4.0*pow((double)(i-closure_duration)/burst_duration-0.5,2));
	for(int i=aspiration_start;i<len;i++)
		excitement[i]+=rrand()*exp(-3.0*(double)(i-aspiration_start)/aspiration_duration)*0.7;
}
//as the name shows
//similar to affricates, but the parameters are a bit different
void generate_plosive(int len,bool aspirated)
{
	excitement.assign(len,0.0);
	int closure_duration=len*0.08;
	int burst_duration=len*0.05;
	if(aspirated)
	{
		int aspiration_start=closure_duration+burst_duration;
		int aspiration_duration=len-aspiration_start;
		for(int i=closure_duration;i<aspiration_start;i++)
			excitement[i]=rrand()*exp(-4.0*pow((double)(i-closure_duration)/burst_duration-0.5,2));
		for(int i=aspiration_start;i<len;i++)
			excitement[i]+=rrand()*exp(-3.0*(double)(i-aspiration_start)/aspiration_duration)*0.5;
	}
	else
	{
		int burst_start=closure_duration;
		int total_burst_duration=len-burst_start;
		for(int i=burst_start;i<len;i++)
			excitement[i]=rrand()*exp(-8.0*(double)(i-burst_start)/total_burst_duration);
	}
}
//note that $(t.frequency_points) will be ignored if $(name) points to a final
short* generate_voice_auto(string name,tone t,bool lowpass_override)
{
	static FFT_filter filt(48000,1024);
	//finals
	if(name=="a"||name=="o"||name=="e"||name=="i"||name=="u"||name=="ve"||name=="un"||
		name=="v"||name=="ee"||name=="ii"||name=="ir"||name=="er"||name=="ou"||name=="ei"||
		name=="ai"||name=="ao"||name=="ia"||name=="ua"||name=="uo"||name=="ui"||
		name=="iu"||name=="iao"||name=="uai"||name=="an"||name=="ian"||name=="uan"||
		name=="van"||name=="en"||name=="in"||name=="vn"||name=="ang"||name=="ie"||
		name=="iang"||name=="uang"||name=="eng"||name=="ing"||name=="ong"||
		name=="iong")
		generate_tone_excitement(t,vector<double>(50,0),1);
	//all sorts of initials
	else if(name=="r"||name=="m"||name=="n"||name=="l"||
			 name=="y"||name=="yy"||name=="w"||name=="-n"||name=="-ng")
		generate_tone_excitement(t,vector<double>(50,0),1);
	else if(name=="s"||name=="sh"||name=="x"||name=="f"||name=="h")
		generate_noise_excitement(t.length);
	else if(name=="z"||name=="zh"||name=="j")
		generate_affricate_excitement(t.length,false);
	else if(name=="c"||name=="ch"||name=="q")
		generate_affricate_excitement(t.length,true);
	else if(name=="b"||name=="d"||name=="g")
		generate_plosive(t.length,false);
	else if(name=="p"||name=="t"||name=="k")
		generate_plosive(t.length,true);
	else if(name=="")
		generate_tone_excitement({{-10000,-10000},t.length},vector<double>(50,0),1);
	else
		throw runtime_error("unknown phoneme \""+name+"\"");
	return generate_voice(name,t.length,lowpass_override);
}
short* crossfade(short *a,size_t a_size,short *b,size_t b_size,size_t crossfade_len)
{
	if(crossfade_len>a_size||crossfade_len>b_size)
		throw runtime_error("crossfade duration is longer than one of the input buffers");
	short* result=new short[a_size+b_size-crossfade_len];
	for(size_t i=0;i<a_size-crossfade_len;i++)
		result[i]=a[i];
	for(size_t i=a_size;i<a_size+b_size-crossfade_len;i++)
		result[i]=b[i-a_size+crossfade_len];
	for(size_t i=a_size-crossfade_len;i<a_size;i++)
	{
		double transition_progress=double(i+crossfade_len-a_size)/crossfade_len;
		result[i]=a[i]*(1-transition_progress)+b[i-a_size+crossfade_len]*transition_progress;
	}
	return result;
}
//split a phoneme into an initial and a final
//$(<ret>.first) will be the initial and $(<ret>.second) will be the final
pair<string,string> split_phoneme(string s)
{
	static const vector<string> initials={"b","p","m","f","d","t","n","l","g","k","h","j","q","x","zh","ch","sh","r","z","c","s","y","w",""};
	static const vector<string> finals=
	{
		"a","o","e","ai","ei","ao","ou","an","en","ang","eng","ong",
		"i","ia","ie","iao","iu","ian","in","iang","ing","iong",
		"u","ua","uo","uai","ui","uan","un","uang","ueng","ue",
		"v","ve","van","vn","er","ee","ii","ir"
	};
	string i,f;
	if(find(initials.begin(),initials.end(),s.substr(0,2))!=initials.end())
	{
		i=s.substr(0,2);
		f=s.substr(2);
		if(find(finals.begin(),finals.end(),f)==finals.end())
			throw runtime_error("unknown final \""+f+"\"");
	}
	else if(find(initials.begin(),initials.end(),s.substr(0,1))!=initials.end())
	{
		i=s.substr(0,1);
		f=s.substr(1);
		if(find(initials.begin(),initials.end(),i)==initials.end())
			throw runtime_error("unknown initial \""+i+"\"");
		if(find(finals.begin(),finals.end(),f)==finals.end())
			throw runtime_error("unknown final \""+f+"\"");
	}
	//special case: no initial present
	//use an empty initial
	else
	{
		if(find(finals.begin(),finals.end(),s)==finals.end())
			throw runtime_error("unknown final \""+s+"\"");
		return make_pair("",s);
	}
	if((i=="zh"||i=="ch"||i=="sh"||i=="r")&&f=="i")
		return make_pair(i,"ir");
	else if((i=="z"||i=="c"||i=="s")&&f=="i")
		return make_pair(i,"ii");
	else if(i=="y")
	{
		if(f=="u")
			return make_pair("yy","v");
		else if(f=="e")
			return make_pair("y","ee");
		else if(f=="ue")
			return make_pair("yy","ve");
		else if(f=="uan")
			return make_pair("yy","van");
		else if(f=="un")
			return make_pair("yy","vn");
		else
			return make_pair("y",f);
	}
	else if((i=="j"||i=="q"||i=="x")&&f[0]=='u')
		return make_pair(i,"v"+f.substr(1));
	else
		return make_pair(i,f);
}
//this function splits a string representing pinyin into the initial-final phoneme and the tone number
//$(<ret>.first) will be the phoneme and $(<ret>.final) will be the tone number
//$(s) will be unchecked, so if it is invalid, the function may crash or crash other functions
//no tone specification defaults to the silent tone(aka 5th tone) 
pair<string,int> split_tone(string s)
{
	if(!isdigit(s[s.length()-1]))
		return make_pair(s,5);
	else
		return make_pair(s.substr(0,s.length()-1),stoi(s.substr(s.length()-1)));
}
//the speed of speech
double speed_factor=1;
//the VOT(which determines the crossfade length) relates mainly to the initial
//so the effect of the final on the VOT is ignored
size_t get_crossfade_length(string initial,int sample_rate=48000)
{
	static const map<string,int> crossfade_duration_ms=
	{
		{"b",15},{"p",25},{"m",40},{"f",25},
		{"d",15},{"t",25},{"n",40},{"l",40},
		{"g",15},{"k",25},{"h",25},
		{"j",20},{"q",30},{"x",25},
		{"zh",20},{"ch",30},{"sh",25},
		{"r",40},{"z",20},{"c",30},{"s",25},
		{"y",40},{"yy",35},{"w",40},{"",0},
	};
	auto it=crossfade_duration_ms.find(initial);
	if (it==crossfade_duration_ms.end())
		throw runtime_error("unknown initial \""+initial+"\"");
	return it->second*sample_rate/1000*speed_factor;
}
const vector<string> affricates={"z","c","zh","ch","j","q"};
const vector<string> fricatives={"f","h","x","s","sh","r"};
const vector<string> plosives={"b","p","d","t","g","k"};
const vector<string> nasals={"m","n","-n","-ng"};
const vector<string> liquids={"l","r"};
const vector<string> glides={"y","yy","w"};
inline bool in(string s,const vector<string> &collection)
{
	return find(collection.begin(),collection.end(),s)!=collection.end();
}
pair<size_t,size_t> calculate_durations(string initial,string final,int tone,double speed_multiplier)
{
	double i_ms,f_ms;//initial and final times in milliseconds 
	if(initial=="")
		i_ms=0;
	else if(in(initial,plosives))
		i_ms=90;
	else if(in(initial,affricates))
		i_ms=120;
	else if(in(initial,fricatives))
		i_ms=150;
	else if(in(initial,nasals))
		i_ms=120;
	else if(in(initial,liquids))
		i_ms=90;
	else if(in(initial,glides))
		i_ms=75;
	else
		i_ms=90;
	//final time depends on tone
	switch(tone)
	{
		case 1:f_ms=150;break;
		case 2:f_ms=120;break;
		case 3:f_ms=150;break;
		case 4:f_ms=130;break;
		case 5:f_ms=160;break;
		default:throw runtime_error("unknown tone "+to_string(tone));
	}
	static double final_time_factor[8]={0,0,1.0,1.05,1.1,1.15,1.13,1.18};//first two should be zero as placeholders
	if(multi_final_mapper.find(final)!=multi_final_mapper.end())
	{
		vector<string> v_list=multi_final_mapper[final];
		f_ms*=final_time_factor[(v_list.size()<<1)|(v_list[v_list.size()-1]=="-n"||v_list[v_list.size()-1]=="-ng")];
	}
	return make_pair(static_cast<size_t>(i_ms*48*speed_multiplier),static_cast<size_t>(f_ms*48*speed_multiplier));
}
inline void set_speech_speed(double speed)
{
	speed_factor=1/speed;
}
short* synthesize_phoneme(string name,size_t &output_len,bool lowpass=true)
{
	pair<string,int> t1=split_tone(name);
	vector<vector<double>> tone_mapper=
	{
		tn_list(t1.second,t1.second),//0th tone used for voiced initials
		tn_list(5,5),
		tn_list(3,5),
		tn_list(2,1.5,1,2.5,3),
		tn_list(5,1),
		tn_list(2.5,2.3,2.1)//a rough approximation of the "silent" tone
	};
	pair<string,string> t2=split_phoneme(t1.first);
	int tone_id=t1.second;
	string initial=t2.first,final=t2.second;
	pair<size_t,size_t> i_f_len=calculate_durations(initial,final,tone_id,speed_factor);
	size_t initial_len=i_f_len.first,final_len=i_f_len.second;
	short* initial_sound=generate_voice_auto(initial,{tone_mapper[0],initial_len},lowpass);
	short* final_sound=generate_voice_auto(final,{tone_mapper[tone_id],final_len},lowpass);
	short* result=crossfade(initial_sound,initial_len,final_sound,final_len,get_crossfade_length(initial));
	output_len=initial_len+final_len-get_crossfade_length(initial);
	delete[] initial_sound;
	delete[] final_sound;
	return result;
}
void write_wav(string filename,vector<short> data)
{
	FILE* fp=fopen(filename.c_str(),"wb");
	static const int sample_rate=48000;
	int data_size=data.size()*sizeof(short);
	int file_size=data_size+36;
	unsigned char header[44]=
	{
		'R','I','F','F',
		(unsigned char)(file_size&0xff),(unsigned char)((file_size>>8)&0xff),(unsigned char)((file_size>>16)&0xff),(unsigned char)((file_size>>24)&0xff),
		'W','A','V','E',
		'f','m','t',' ',
		16,0,0,0,//size of header
		1,0,//PCM format
		1,0,//mono
		sample_rate&0xff,(sample_rate>>8)&0xff,(sample_rate>>16)&0xff,(sample_rate>>24)&0xff,//sample rate
		(sample_rate<<1)&0xff,(sample_rate>>7)&0xff,(sample_rate>>15)&0xff,(sample_rate>>23)&0xff,//bytes per second
		2,0,//block alignment
		16,0,//16-bit sample
		'd','a','t','a',
		(unsigned char)(data_size&0xff),(unsigned char)((data_size>>8)&0xff),(unsigned char)((data_size>>16)&0xff),(unsigned char)((data_size>>24)&0xff)
	};
	fwrite(header,44,1,fp);
	fwrite(reinterpret_cast<const char*>(data.data()),data_size,1,fp);
	fclose(fp);
}

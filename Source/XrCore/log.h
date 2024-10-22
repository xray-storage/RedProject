#ifndef logH
#define logH
#include <string_view>
#include <format>

#define VPUSH(a)	((a).x), ((a).y), ((a).z)

void 	XRCORE_API	__cdecl		Msg	(LPCSTR format, ...);

void 	XRCORE_API		Log(LPCSTR msg);
void 	XRCORE_API		Log(LPCSTR msg);
void 	XRCORE_API		Log(LPCSTR msg, LPCSTR			dop);
void 	XRCORE_API		Log(LPCSTR msg, u32			dop);
void 	XRCORE_API		Log(LPCSTR msg, int  			dop);
void 	XRCORE_API		Log(LPCSTR msg, float			dop);

void 	XRCORE_API		Log			(LPCSTR msg, const Fvector& dop);
void 	XRCORE_API		Log			(LPCSTR msg, const Fmatrix& dop);
void 	XRCORE_API		LogWinErr	(LPCSTR msg, long 			err_code);

typedef void	( * LogCallback)	(LPCSTR string);
LogCallback	XRCORE_API			SetLogCB	(LogCallback cb);
extern 	XRCORE_API	BOOL						LogExecCB;

template <typename... Args>
inline void LogFormat(xr_string rt_fmt_str, Args&&... args)
{
	Log(std::vformat(rt_fmt_str.c_str(), std::make_format_args(args...)).data());
}
#endif


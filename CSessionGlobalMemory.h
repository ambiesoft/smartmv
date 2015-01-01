#pragma once

template<class T>
class CSessionGlobalMemory
{
public:
	explicit CSessionGlobalMemory(LPCSTR pName) {
		h_=NULL;
		p_=NULL;
		m_=NULL;

		size_t len=lstrlenA(pName);
		m_pName = (LPSTR)LocalAlloc(LMEM_FIXED, len+sizeof(char));
		lstrcpyA(m_pName, pName);

		m_pMutexName = (LPSTR)LocalAlloc(LMEM_FIXED, len+sizeof("_Mutex")-1+sizeof(char));
		lstrcpyA(m_pMutexName, pName);
		lstrcatA(m_pMutexName, "_Mutex");

	}
	~CSessionGlobalMemory(){
		release();
	}
 
	operator T() {
		return get();
	}
 
	void operator =(const T& t) {
		set(t);
		//return t;
	}

	

private:
	class Locker {
		public:
			HANDLE m_;
			Locker(HANDLE m) {
				m_=m;
				WaitForSingleObject(m,INFINITE);
			}
			~Locker() {
				ReleaseMutex(m_);
			}
	};

	void release() {
		if(p_)
		{
			UnmapViewOfFile(p_);
			p_=NULL;
		}

		if(h_)
		{
			CloseHandle(h_);
			h_=NULL;
		}

		if(m_)
		{
			CloseHandle(m_);
			m_=NULL;
		}
		LocalFree(m_pName);
		LocalFree(m_pMutexName);
	}
	void ensure() {
		if(!m_)
		{
			m_=CreateMutexA(NULL,FALSE,m_pMutexName);
		}

		bool first=false;
		if(!h_)
		{
			h_=CreateFileMappingA(
				INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				0,sizeof(T),
				m_pName);
			if(h_ && GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				first = true;
			}
		}
		if(!p_)
		{
			p_=MapViewOfFile(
				h_,
				FILE_MAP_READ|FILE_MAP_WRITE,
				0,0,
				sizeof(T));
		}

		if(first)
		{
			Locker l(m_);
			if(first)
			{
				memset(p_,0,sizeof(T));
				first=false;
			}
		}
	}

	T get() {
 		ensure();
		Locker l(m_);
		T t;
		memcpy(&t,p_,sizeof(t));

		return t;
	}

	void set(const T& t) {
		ensure();
		Locker l(m_);
		memcpy(p_, &t, sizeof(t));
	}

	bool first_;
	HANDLE h_;
	void* p_;
	HANDLE m_;
	LPSTR m_pName;
	LPSTR m_pMutexName;
};


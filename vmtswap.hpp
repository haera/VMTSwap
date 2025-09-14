#pragma once
#include <stdexcept>

class VMTSwap
{
private:
	void**	    m_ppOrigVMT;  // pointer to the VMT pointer (e.g., VPTR)
	uintptr_t*  m_pOrigVMT;   // original VMT
	uintptr_t*  m_pCopyVMT;   // copied (shadow) VMT

	size_t      m_szVMT = 0;  // num of virtual functions in vtable 

	/**
	 * calculates the num of functions in VMT
	 * @return size of VMT
	*/
	size_t calc_vmt_size()
	{
		if (!m_pOrigVMT)
			return 0;

		// could also query vmmem, but no more winapi for now
		size_t fnCount = 0;
		while (m_pOrigVMT[fnCount] != 0)
			fnCount++;

		return fnCount;
	}

public:
	/**
	 * creates shadow copy of object's VMT and swaps it in
	 * @param ptr: pointer to instance of object to be hooked
	*/
	VMTSwap(void* ptr)
	{
		if (!ptr)
			throw std::invalid_argument("ptr cannot be null");

		// save the orig vtable
		m_ppOrigVMT = static_cast<void**>(ptr);
		m_pOrigVMT = *reinterpret_cast<uintptr_t**>(ptr);

		m_szVMT = calc_vmt_size();

		// alloc new vtable and copy the original over
		if (!m_szVMT)
			throw std::runtime_error("vmt size is zero");

		m_pCopyVMT = new uintptr_t[m_szVMT];
		memcpy(m_pCopyVMT, m_pOrigVMT, m_szVMT * sizeof(uintptr_t));

		// VPTR ptr swap to shadow VMT
		*m_ppOrigVMT = m_pCopyVMT;
	}

	/**
	 * restores original VMT, frees shadow VMT
	 */
	~VMTSwap()
	{
		*m_ppOrigVMT = m_pOrigVMT; // swap VPTR back to original VMT
		delete m_pCopyVMT;
	}

	/**
	 * swaps function in shadow VMT with callback
	 * @brief note: use call_orig_func in your callback to ret to original function
	 * @param idx: index of function in VMT
	 * @param fnCallback: pointer to your own function
	 */
	void hook(int idx, void* fnCallback)
	{
		if (idx < 0 || idx >= m_szVMT)
			throw std::range_error("vtable index out of bounds");

		// overwrite function ptr in shadow VMT w/ callback
		m_pCopyVMT[idx] = reinterpret_cast<uintptr_t>(fnCallback);
	}

	/**
	 * swaps function in shadow VMT with original
	 * @brief note: only deconstructor restores the entire vtable
	 * @param idx: index of function in VMT
	 */
	void unhook(int idx)
	{
		if (idx < m_szVMT && idx >= 0)
			m_pCopyVMT[idx] = m_pOrigVMT[idx]; // restore original function ptr in shadow VMT
	}

	/**
	 * gets function at idx from original VMT
	 * @tparam T: original object type
	 * @param idx: index of function in VMT
	 * @return callable type for original function signature
	 */
	template <typename T>
	T get_orig_func(int idx)
	{
		return reinterpret_cast<T>(m_pOrigVMT[idx]);
	}

};

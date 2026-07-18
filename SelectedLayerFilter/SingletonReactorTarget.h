#pragma once

#include "acdocman.h"
#include "DocumentReactorTarget.h"

// Template Parameter T is the concrete derived class
// Template Parameter D must be a class derived from CDocumentData

template <typename T, typename D = CDocumentData> 
class DocumentReactorTargetSingleton : public CDocumentReactorTarget<D>
{

	friend class DocumentReactorTargetSingleton<T, D>;

protected:

	DocumentReactorTargetSingleton() : CDocumentReactorTarget<D>(false)
	{
	}

public:

	virtual ~DocumentReactorTargetSingleton() 
	{
		// InternalDestroy();
		//if( s_Container.m_pInstance == this )
		//	s_Container.m_pInstance = NULL;
	}

	static T* GetInstance( bool create = false, bool initialize = false )
	{
		if( s_Container.m_pInstance == NULL && create )
		{
			T* pT = new T();
			s_Container.m_pInstance = pT;
			if( initialize )
				pT->Initialize();
		}
		return s_Container.m_pInstance;
	}

	static T* Create( bool bInitialize = false )
	{
		return GetInstance( true, bInitialize );
	}

	static bool InstanceInitialized()
	{
		return s_Container.m_pInstance != NULL;
	}

	static void InitializeInstance()
	{
		Create(true);
	}

	static void Terminate()
	{
		InternalDestroy();
	}

private:

	static void InternalDestroy()
	{
		if( s_Container.m_pInstance != NULL )
		{
			s_Container.m_pInstance->Delete();
			// delete s_Container.m_pInstance;
			s_Container.m_pInstance = NULL;
		}
	}

	typedef struct tagContainer
	{
	public:
		tagContainer() : m_pInstance(NULL) {}
		~tagContainer()
		{
			if( m_pInstance != NULL )
				delete m_pInstance;
			m_pInstance = NULL;
		}

		T* m_pInstance;

	} Container;

	static Container s_Container;

};

template <class T, class D>
typename DocumentReactorTargetSingleton<T,D>::Container DocumentReactorTargetSingleton<T,D>::s_Container;

// -----------------------------------------------------------------------------


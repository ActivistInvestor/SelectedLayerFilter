#pragma once

#include "acdocman.h"

class CDocumentData
{
protected:

	AcApDocument* m_pDoc;

public:

	CDocumentData () : m_pDoc(NULL) 
	{
	}

	CDocumentData (const CDocumentData& data) 
	{ 
		CopyFrom( data );
	}

	~CDocumentData () 
	{
	}

	virtual void CopyFrom( const CDocumentData& other )
	{
		this->m_pDoc = other.m_pDoc;
	}

	virtual void Attach( AcApDocument* pDoc )
	{
		m_pDoc = pDoc;
	}

	virtual void Detach()
	{
		m_pDoc = NULL;
	}

	virtual void Activated()
	{
	}

	AcApDocument* document()
	{
		return m_pDoc;
	}

};

template <class T = CDocumentData>      // T must be derived from DocumentData
class CDocumentDataMap
{
public:
	CDocumentDataMap() 
	{
	}
	
	virtual ~CDocumentDataMap()
	{
		Clear();
	};

	virtual T& Add( AcApDocument* pDoc)
	{
		return docData(pDoc);
	}

	/*
	virtual T& Add( AcApDocument* pDoc )
	{
		if( ! Contains( pDoc ) )
			return InternalAdd(pDoc);
		else
			return 
		{
			T* pT = new T();
			pT->Attach( pDoc );
			m_array.append(pT);
		}
		return docData( pDoc );
	}
	*/

	virtual void Remove(AcApDocument* pDoc)
	{
		int i = IndexOf( pDoc );
		if( i > -1 )
		{
			T* p = m_array[i];
			m_array.removeAt(i);
			// assert( p != NULL );
			if( p != NULL )
			{
				p->Detach();
				delete p;
			}
		}
	}

	void Clear()
	{
		int cnt = m_array.length();
		for( int i = 0; i < cnt; i++ )
		{
			T* p = m_array[i];
			if( p != NULL )
			{
				p->Detach();
				delete p;
			}
		}
		m_array.setLogicalLength(0);
	}

	T& docData( AcApDocument *pDoc ) 
	{
		int i = IndexOf(pDoc);
		if( i < 0 )
			return InternalAdd(pDoc);
		else
			return (T&) *(m_array[i]);
	}

	T& docData( AcDbDatabase *pDb ) 
	{
		int i = IndexOfDatabase( pDb );
		if( i < 0 )
		{
			AcApDocument* pDoc = acDocManager->document( pDb );
			if( pDoc != NULL )
				return InternalAdd( pDoc );
			return NULL;
		}
		else
			return (T&) *(m_array[i]);
	}

	T& docData () 
	{
		return docData(acDocManager->curdocument());
	}

	inline bool Contains( AcApDocument* pDoc ) const
	{
		return IndexOf(pDoc) > -1;
	}

	inline int IndexOf( AcApDocument* pDoc ) const
	{
		int cnt = m_array.length();
		for( int i = 0; i < cnt; i++ )
			if( m_array[i]->document() == pDoc )
				return i;
		return -1;
	}

	inline int IndexOf( AcDbDatabase* pDb ) const
	{
		int cnt = m_array.length();
		for( int i = 0; i < cnt; i++ )
			if( m_array[i]->document()->database() == pDb )
				return i;
		return -1;
	}

	T& operator[](AcApDocument* pDoc) 
	{
		return docData(pDoc);
	}

	T& operator[](int index) 
	{
		return (T&) (*m_array[index]);
	}

	int length() const
	{
		return m_array.length();
	}

	typedef typename AcArray<T*> ArrayType;

	void GetData( ArrayType& data )
	{
		data.append( m_array );
	}

private:

	virtual T& InternalAdd( AcApDocument* pDoc )
	{
		T* pT = new T();
		m_array.append(pT);
		pT->Attach( pDoc );
		// return (T&) *(m_array.last());
		return (T&) *(m_array.last());
	}

	typedef typename AcArray<T*> ArrayType;

	AcArray<T*> m_array;

};

template <class D = CDocumentData>
class CDocumentReactorTarget : public AcApDocManagerReactor
{
	typedef CDocumentDataMap<D> DocDataMap;

public:

	CDocumentReactorTarget( bool bInitialize = false ) 
		: m_bInitialized(false), m_pLastActiveDoc(NULL)
	{
		if( bInitialize )
			Initialize();  
	}

	virtual ~CDocumentReactorTarget() 
	{
		Uninitialize();
	}

	// This should be called before deleting the instance,
	// because it calls a virtual that will not be called
	// if this is called from the destructor.
	void Uninitialize()
	{
		if( m_bInitialized )
		{
#ifdef EXTRACE
			ExTrace(_T("CDocumentReactorTarget::Uninitialize()"));
#endif
			m_bInitialized = false;
			try
			{
				AcArray<D*> data;
				m_dataMap.GetData( data );
				int cnt = data.length();
				for( int i = 0; i < cnt; i++ )
				{
					RemoveDocument( data[i]->document() );  // Calling virtual from destructor
				}
				m_dataMap.Clear();
				if( ! AcadIsQuitting() )
					acDocManager->removeReactor(this);
			}
			catch(...) 
			{
#ifdef EXTRACE
				ExTrace(_T("Exception in CDocumentReactorTarget::Uninitialize()"));
#endif
			}
		}
	}

	void Delete()
	{
		Uninitialize();
		delete this;
	}

	void Initialize()
	{
		if( ! m_bInitialized )
		{
			OnBeginInitialize();
			if( acDocManager->documentCount() > 0 )
			{
				AcApDocument* pCurrent = acDocManager->curDocument();
				AcApDocumentIterator* pIter = acDocManager->newAcApDocumentIterator();
				if( pIter != NULL )
				{
					for( AcApDocument* pDoc = pIter->document(); !pIter->done(); pIter->step() )
					{
						D& temp = InternalAdd( pDoc );
						if( pDoc == pCurrent ) // TODO: Test if doc is untitled
							temp.Activated();
					}
					delete pIter;
				}
			}
			acDocManager->addReactor( this );
			m_bInitialized = true;
			OnEndInitialize();
		}
	}

	bool IsInitialized()
	{
		return m_bInitialized;
	}

	D& docData( AcApDocument *pDoc ) 
	{
		return m_dataMap[pDoc];
	}

	D& docData () 
	{
		return m_dataMap[acDocManager->curDocument()];
	}

	D& operator[](AcApDocument* pDoc)
	{
		return m_dataMap[pDoc];
	}

protected:

	typedef CDocumentReactorTarget<D> CDocumentReactorTargetBase;

	virtual void OnBeginInitialize()
	{
	}

	virtual void OnEndInitialize()
	{
	}

	virtual void documentToBeDestroyed( AcApDocument *pDoc )
	{
		if( pDoc != NULL )
		{
			RemoveDocument(pDoc);
			m_dataMap.Remove(pDoc);
		}
	}

	virtual void documentCreated( AcApDocument* pDocCreating )
	{
		if( pDocCreating != NULL )
		{
			InternalAdd( pDocCreating );
		}
	}

	virtual void documentActivated( AcApDocument* pActivatedDoc )
	{
		if( m_pLastActiveDoc != pActivatedDoc && pActivatedDoc != NULL )
			m_dataMap[pActivatedDoc].Activated();
		m_pLastActiveDoc = pActivatedDoc;
	}

	virtual void AddDocument( AcApDocument* pDoc )
	{
	}

	virtual void RemoveDocument( AcApDocument* pDoc )
	{
	}

private:

	AcApDocument* m_pLastActiveDoc;

	D& InternalAdd( AcApDocument* pDoc )
	{
		D& temp = m_dataMap.Add( pDoc );
		AddDocument( pDoc );
		return temp;
	}

	void InternalRemove( AcApDocument* pDoc )
	{
		RemoveDocument( pDoc );
	}

	bool m_bInitialized;

	DocDataMap m_dataMap;
};



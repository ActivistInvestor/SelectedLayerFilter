#pragma once
// #include "acly.h"

class ChildFilter;
class CustomLayerFilter :	public AcLyLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( CustomLayerFilter );

	inline CustomLayerFilter(void) : m_bNestedCreated( false ) // : m_bAllowNested(false) 
	{
		// setName( _T("CustomLayerFilter") );
		//generateNested();
	}

	inline virtual const ACHAR * name() const
	{
		return _T("CustomLayerFilter");
	}

	inline virtual Acad::ErrorStatus setName(const ACHAR *)
	{
		return Acad::eOk;
	}


	//inline CustomLayerFilter( const TCHAR* name )
	//{
	//	setName( name );
	//}

	inline virtual ~CustomLayerFilter(void)
	{
		deleteNested();
	}

	inline void deleteNested()
	{
		//if( m_bNestedCreated )
		//{
		//	int len = m_nested.length();
		//	for(int i=0; i<len; i++)
		//	{
		//		AcLyLayerFilter* filter = m_nested[i];
		//		if( filter != NULL )
		//			delete filter;
		//	}
		//	m_nested.setLogicalLength(0);
		//}
	}

	inline bool allowRename() const
	{
		return false;
	}

	inline bool allowDelete() const
	{
		return false;
	}

	inline bool isDynamicallyGenerated() const
	{
		return false;
	}

	inline DialogResult showEditor()
	{
		return kCancel;
	}

	inline virtual bool allowNested() const
	{
		return true;
	}

	inline virtual Acad::ErrorStatus generateNested();

	//inline virtual const AcArray<AcLyLayerFilter*>& getNestedFilters() const
	//{
	//	if( ! m_bNestedCreated )
	//	{
	//		const_cast<CustomLayerFilter*>( this )->generateNested();
	//	}
	//	return m_nested;
	//}

	//inline virtual Acad::ErrorStatus addNested(AcLyLayerFilter* filter)
	//{
	//	m_nested.append(filter);
	//	return Acad::eOk;
	//}

	//inline virtual Acad::ErrorStatus removeNested(AcLyLayerFilter* filter)
	//{
	//	m_nested.remove(filter);
	//	return Acad::eOk;
	//}

	//{
	//	return AddChildFilter( new ChildFilter() );
	//}

	//virtual bool filter( AcDbLayerTableRecord* layer )
	//{
	//	return true;
	//}


	//Acad::ErrorStatus AddChildFilter( AcLyLayerFilter* filter )
	//{
	//	m_bAllowNested = true;
	//	Acad::ErrorStatus es = this->addNested( filter );
	//	m_bAllowNested = false;
	//	return es;
	//}


private:
	// bool m_bAllowNested;

	AcArray<AcLyLayerFilter*> m_nested;
	bool m_bNestedCreated;
};


class ChildFilter :	public AcLyLayerFilter
{
public:

	ACRX_DECLARE_MEMBERS( ChildFilter );

	inline ChildFilter(void) // : m_pParent(NULL) // : CustomLayerFilter(_T("ChildFilter"))
	{
	}

	inline virtual ~ChildFilter(void)
	{
	}

	inline virtual const ACHAR * name() const
	{
		return _T("ChildCustomLayerFilter");
	}



	//inline virtual Acad::ErrorStatus setName(const ACHAR *)
	//{
	//	return Acad::eOk;
	//}

	inline virtual bool isDynamicallyGenerated()
	{
		return true;
	}

	inline bool allowRename() const
	{
		return false;
	}

	inline bool allowDelete() const
	{
		return false;
	}

	inline DialogResult showEditor()
	{
		return kCancel;
	}

	inline virtual bool allowNested() const
	{
		return false;
	}

	//inline virtual AcLyLayerFilter* parent() const
	//{
	//	return m_pParent;
	//}

	//inline void setParent( AcLyLayerFilter* parent )
	//{
	//	m_pParent = parent;
	//}

private:
	// AcLyLayerFilter* m_pParent;

};
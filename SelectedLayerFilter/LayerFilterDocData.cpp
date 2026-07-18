#pragma once
#include "StdAfx.h"
#include "LayerFilterDocData.h"
#include "SelectionLayerFilter.h"
#include "NestedSelectionLayerFilter.h"
#include "RecentLayerFilter.h"
#include "LayerFilterManager.h"
// #include "BlockLayerFilter.h"
#include <tchar.h>
// #include "Banner.h"

#include "CustomLayerFilter.h"

LayerFilterDocData::LayerFilterDocData() 
	: m_bUpdating( false ), m_bDirty( true ), m_bIsModal( false ), m_bEnabled( true )
{
}

LayerFilterDocData::~LayerFilterDocData()
{
}

void LayerFilterDocData::Attach( AcApDocument* pDoc )
{
	CDocumentData::Attach( pDoc );

	if( pDoc->inputPointManager() != NULL )
		pDoc->inputPointManager()->addInputContextReactor( this );

	if( ! expired() )
	{
		AddLayerFilters();
	}
	else
	{
		m_bEnabled = false;
		acutPrintf(_T("\nThis preview release of QLayer has expired."));
	}
}

void LayerFilterDocData::Detach()
{
	if( this->document() != NULL && this->document()->inputPointManager() != NULL )
	{
		this->document()->inputPointManager()->removeInputContextReactor( this );
	}
}

void LayerFilterDocData::AddLayerFilters()
{
	AcApDocument* pDoc = this->document();
	if( pDoc != NULL )
	{
		AcLyLayerFilterManager* pMgr = aclyGetLayerFilterManager( pDoc->database() );
		if( pMgr != NULL )
		{
			AcLyLayerFilter* pRoot = NULL;
			AcLyLayerFilter* pCurrent = NULL;
			Acad::ErrorStatus es = pMgr->getFilters( pRoot, pCurrent );
			if( es == Acad::eOk )
			{
				bool filtersChanged = false;
				if( pRoot != NULL )
				{
/*
					if( SelectionLayerFilter::Find( pRoot ) == NULL )
					{
						SelectionLayerFilter* pFilter = new SelectionLayerFilter();
						if( pRoot->addNested( pFilter ) != Acad::eOk )
							delete pFilter;
						else
						{
							filtersChanged = true;
							//if( NestedSelectionLayerFilter::Find( pFilter ) == NULL )
							//{
							//	NestedSelectionLayerFilter* pNested = new NestedSelectionLayerFilter();
							//	if( pFilter->Add( pNested ) != Acad::eOk )
							//		delete pNested;
							//	else
							//		filtersChanged = true;
							//}
						}
					}
#ifdef EXTRACE
					else
						ExTrace(_T("SelectionLayerFilter found."));
#endif
					if( RecentLayerFilter::Find( pRoot ) == NULL )
					{
						RecentLayerFilter* pFilter = new RecentLayerFilter();
						if( pRoot->addNested( pFilter ) != Acad::eOk )
							delete pFilter;
						else
							filtersChanged = true;
					}
#ifdef EXTRACE
					else
						ExTrace(_T("RecentLayerFilter found."));
#endif
*/
					AcArray<AcLyLayerFilter*> filters = pRoot->getNestedFilters();

					int cnt = filters.length();

					CustomLayerFilter* filter = NULL;
					for(int i = 0; i < cnt; i++)
					{
						filter = CustomLayerFilter::cast( filters[i] );
						if( filter != NULL )
							break;
					}

					if( filter == NULL )
					{
						filter = new CustomLayerFilter(); // (_T("MyCustomLayerFilter"));
						filtersChanged = true;

						//AcArray<AcLyLayerFilter*> children = filter->getNestedFilters();
						//if( children.length() == 0 )
						//{
						//	CustomLayerFilter* childFilter = new CustomLayerFilter(_T("Child Filter"));
						//	filter->AddChildFilter( childFilter );
						//	filtersChanged = true;
						//}
						//else
						//{
						//	CustomLayerFilter* childFilter = CustomLayerFilter::cast(children[0]);
						//	if( childFilter != NULL )
						//		ExTrace(_T(">>>> Found nested CustomLayerFilter %s"), childFilter->name());
						//}


						pRoot->addNested( filter );
					}
					else
					{
						ExTrace(_T(">>>> Found CustomLayerFilter %s"), filter->name());
						AcArray<AcLyLayerFilter*> children = filter->getNestedFilters();
						if( children.length() > 0 )
						{
							CustomLayerFilter* childFilter = CustomLayerFilter::cast(children[0]);
							if( childFilter != NULL )
							ExTrace(_T(">>>> Found nested CustomLayerFilter %s"), childFilter->name());
						}
					}

					if( filtersChanged && acDocManager->lockDocument( pDoc ) == Acad::eOk )
					{
						pDoc->pushDbmod();
						es = pMgr->setFilters( pRoot, pCurrent );
						if( es == Acad::eOk && LayerFilterManager::IsLayerPropertiesManagerActive() )
							UpdateThunk( pDoc->database() );
						pDoc->popDbmod();
						acDocManager->unlockDocument( pDoc );
					}
					delete pRoot;
				}
			}
		}
	}
}

bool LayerFilterDocData::filterSelectedLayer( AcDbLayerTableRecord* layer )
{
	if( ! m_bEnabled )
		return true;
	UpdatePickfirstLayers();
	return m_layers.Contains( layer->objectId() );
}

bool LayerFilterDocData::filterNestedLayer( AcDbLayerTableRecord* layer )
{
	if( ! m_bEnabled )
		return true;
	UpdateNestedPickfirstLayers();
	return m_nestedLayers.Contains( layer->objectId() );
}

bool LayerFilterDocData::filterRecentLayer( AcDbLayerTableRecord* layer )
{
	if( ! m_bEnabled )
		return true;
	return m_recent.contains( layer->objectId() );
}

// -----------------------------------------------------------------------------
void LayerFilterDocData::OnPickfirstModified()
{
	if( m_bEnabled && ! m_bIsModal )
	{
		m_bDirty = true;
		m_bNestedDirty = true;
		m_layers.Clear();
		m_nestedLayers.Clear();
		UpdateLayerPropertiesManager();
	}
}

void LayerFilterDocData::OnModelessOperationStarting( const TCHAR* context )
{
#ifdef EXTRACE
	ExTrace( _T("ModelessOperationStarting: %s"), context );
#endif
}

void LayerFilterDocData::OnModelessOperationEnded( const TCHAR* context )
{
	if( m_bEnabled && _tcsicmp( context, _T("OPM_CHGPROP") ) == 0 )
		OnPickfirstModified();
#ifdef EXTRACE
	else
		ExTrace( _T("ModelessOperationEnded: %s"), context );
#endif
}

void LayerFilterDocData::BeginModal( resbuf* rb )
{
	if( rb != NULL && rb->restype == RTPICKS )
	{
		m_bDirty = true;
		m_bNestedDirty = true;
		m_bIsModal = true;
		CollectSelectedLayerIds( rb->resval.rlname );
		CollectNestedLayerIds();
	}
}

void LayerFilterDocData::EndModal()
{
	m_bIsModal = false;
	m_bNestedDirty = false;
	m_bDirty = false;
	OnPickfirstModified();
}

void LayerFilterDocData::UpdateNestedPickfirstLayers( bool UpdateUi )
{
	UpdatePickfirstLayers();
	if( m_bNestedDirty && ! m_bIsModal )
	{
		CollectNestedLayerIds();
		if( UpdateUi )
			UpdateLayerPropertiesManager();
	}
}

void LayerFilterDocData::UpdatePickfirstLayers( bool UpdateUi )
{
	if( m_bDirty && ! m_bIsModal )
	{
		resbuf* unused = NULL;
		resbuf* rb = NULL;
		if( acedSSGetFirst( &unused, &rb ) == RTNORM && rb != NULL )
		{
			CollectSelectedLayerIds( rb->resval.rlname );
			acedSSFree( rb->resval.rlname );
			acutRelRb( rb );
			if( unused != NULL )
				acutRelRb( unused );
			if( UpdateUi )
				UpdateLayerPropertiesManager();
		}
	}
}

int LayerFilterDocData::CollectSelectedLayerIds( ads_name& ss )
{
	m_layers.Clear();
	m_blocks.Clear();
	m_nestedLayers.Clear();

	// m_blockMap.Clear();

	long len = 0L;
	if( acedSSLength( ss, &len ) == RTNORM && len > 0L )
	{
		for( int i = 0; i < len; i++ )
		{
			ads_name e;
			if( acedSSName( ss, i, e ) == RTNORM )
			{
				AcDbObjectId id;
				if( acdbGetObjectId( id, e ) == Acad::eOk )
				{
					AcDbEntityPointer pEnt( id, AcDb::kForRead );
					if( pEnt.openStatus() == Acad::eOk && pEnt->visibility() == AcDb::kVisible )
						m_layers.Add( pEnt->layerId() );

					AcDbBlockReference* pBlkRef = AcDbBlockReference::cast( pEnt );
					if( pBlkRef != NULL )
					{
						id = GetBlockTableRecord( pBlkRef );
						if( ! id.isNull() )
							m_blocks.Add( id );
					}
				}
			}
		}
	}
	m_bDirty = false;
	m_bNestedDirty = true;
#ifdef EXTRACE	
	ExTrace(_T("Selected Layers Filter updated ( blocks = %d )"), m_blocks.Count() );
#endif
	return m_layers.Count();
}

int LayerFilterDocData::CollectNestedLayerIds()
{
	m_nestedLayers.Clear();
	// m_blockMap.Clear();
	AcDbObjectIdSet visited;
	AcDbObjectIdArray ids;
	m_blocks.GetObjectIds( ids );
	int cnt = ids.length();
	for( int i = 0; i < cnt; i++ )
	{
		if( visited.Add( ids[i] ) )
			CollectBlockLayers( ids[i], visited );
	}
	m_bNestedDirty = false;
#ifdef EXTRACE
	ExTrace(_T("Nested Layers Filter updated"));
#endif

	////////////////// TESTING ONLY ///////////////////

	//AcDbObjectIdArray keys;
	//m_blockMap.GetKeys( keys );

	//ExTrace(_T("keys.length() == %d"), keys.length());


	//int c = m_blockMap.Count();
	//ExTrace(_T("m_blockMap.Count() == %d"), c);
	//
	//BlockLayerMap::Iterator it = m_blockMap.Begin();
	//BlockLayerMap::Iterator endit = m_blockMap.End();
	//while( it != m_blockMap.End() )
	//{
	//	AcDbObjectId id = (*it).first;
	//	AcDbObjectIdSet& idset = (*it).second;
	//	AcDbBlockTableRecordPointer pBtr( id, AcDb::kForRead );
	//	if( pBtr.openStatus() == Acad::eOk )
	//	{
	//		const TCHAR* name;
	//		pBtr->getName( name );
	//		int cnt = idset.Count();
	//		ExTrace(_T("Block Layers[%s].Count = %d"), name, cnt);
	//	}
	//	++it;
	//}

	//for( BlockLayerMap::Iterator it = m_blockMap.Begin(); it != m_BlockMap.End(); it++ )
	//{
	//	AcDbObjectId id = (*it).first;
	//	AcDbObjectIdSet& idset = (*it).second;
	//	AcDbBlockTableRecordPointer pBtr( id, AcDb::kForRead );
	//	if( pBtr.openStatus() == Acad::eOk )
	//	{
	//		const TCHAR* name;
	//		pBtr->getName( name );
	//		int cnt = idSet.Count();
	//		ExTrace(_("Block Layers[%s].Count = %d"), name, cnt);
	//	}
	//}


	return m_nestedLayers.Count();
}

AcDbObjectId LayerFilterDocData::GetBlockTableRecord( AcDbBlockReference* blockref )
{
	if( blockref->visibility() == AcDb::kInvisible )
		return AcDbObjectId::kNull;
	AcDbDynBlockReference dblock(blockref);
	if( dblock.isDynamicBlock() )
		return dblock.dynamicBlockTableRecord();
	AcDbBlockTableRecordPointer pRec( blockref->blockTableRecord(), AcDb::kForRead );
	if( pRec->isAProxy() || pRec->isFromExternalReference() || pRec->isFromOverlayReference() || pRec->isLayout() )
		return AcDbObjectId::kNull;
	return blockref->blockTableRecord();
}

// Get the layers of all objects in the block table record param
void LayerFilterDocData::CollectBlockLayers( AcDbObjectId& blockId, AcDbObjectIdSet& visited )
{
	AcDbBlockTableRecordPointer pRec( blockId, AcDb::kForRead );
	AcDbBlockTableRecordIterator* pIter = NULL;
	// AcDbObjectIdSet& blockLayers = m_blockMap[blockId];
	if( pRec.openStatus() == Acad::eOk )
	{
		Acad::ErrorStatus es = pRec->newIterator( pIter );
		if( es == Acad::eOk && pIter != NULL )
		{
			AcDbObjectId lastLayer;
			for( ; ! pIter->done(); pIter->step() )
			{
				AcDbObjectId id;
				es = pIter->getEntityId( id );
				if( es == Acad::eOk )
				{
					AcDbEntityPointer pEnt(id, AcDb::kForRead);
					if( pEnt.openStatus() == Acad::eOk && pEnt->visibility() == AcDb::kVisible )
					{
						AcDbObjectId layerId = pEnt->layerId();

						// blockLayers.Add( layerId );

						if( lastLayer != layerId && m_nestedLayers.Add( layerId ) )
							lastLayer = layerId;
						AcDbBlockReference* pBlockRef = AcDbBlockReference::cast( pEnt );
						if( pBlockRef != NULL )
						{
							AcDbObjectId nested = GetBlockTableRecord( pBlockRef );
							if( ! nested.isNull() && visited.Add( nested ) )
								CollectBlockLayers( nested, visited );
						}
					}
				}
			}
		}
	}
	if( pIter != NULL )
		delete pIter;
}

bool LayerFilterDocData::UpdateNeeded()
{
	AcApDocument* pDoc = this->document();
	bool result = false;
	if( pDoc != NULL )
	{
		AcLyLayerFilterManager* pMgr = aclyGetLayerFilterManager( pDoc->database() );
		if( pMgr != NULL )
		{
			AcLyLayerFilter* pRoot = NULL;
			AcLyLayerFilter* pCurrent = NULL;
			Acad::ErrorStatus es = pMgr->getFilters( pRoot, pCurrent );
			if( es == Acad::eOk )
			{
				if( pRoot != NULL )
				{
					result = pCurrent == NULL 
						|| NestedSelectionLayerFilter::cast( pCurrent ) != NULL
						|| SelectionLayerFilter::cast( pCurrent ) != NULL;
					if( pCurrent != NULL )
						ExTrace(_T("\n  *** Current filter: %s"), pCurrent->name());
					delete pRoot;
				}
			}
		}
	}
	ExTrace(_T("\n  UpdateNeeded(): %s"), result ? _T("true") : _T("false"));
	return result;
}

void LayerFilterDocData::UpdateLayerPropertiesManager()
{
	AcApDocument* pDoc = this->document();
	if( pDoc != NULL && ! m_bUpdating )
	{
		if( LayerFilterManager::IsLayerPropertiesManagerActive() && UpdateNeeded() )
		{
#ifdef EXTRACE
			ExTrace(_T("Updating Layer UI"));
#endif
			if( acDocManager->lockDocument( pDoc ) == Acad::eOk )
			{
				AcDbDatabase* pDb = pDoc->database();
				pDoc->pushDbmod();
				UpdateThunk( pDb );
				pDoc->popDbmod();
				acDocManager->unlockDocument( pDoc );
			}
		}
	}
}

void LayerFilterDocData::UpdateThunk( AcDbDatabase* pDb )
{
	m_bUpdating = true;
	pDb->disableUndoRecording(true);
	pDb->setClayer( pDb->clayer() );
	pDb->disableUndoRecording(false);
	m_bUpdating = false;
}

void LayerFilterDocData::beginQuiescentState()
{
	if( this->document() != NULL )
	{
		AcDbObjectId id = this->document()->database()->clayer();
		if( m_clayer != id && ! m_recent.contains( id ) )
		{
			m_recent.insertAt( 0, id );
			if( m_recent.length() > 10 )
				m_recent.removeLast();
			m_clayer = id;            // update needed if recent layer filter is active
		}
	}
}

bool LayerFilterDocData::IsModalLayerDialogActive()
{
	bool result = false;
	ACHAR* name = NULL;
	Acad::ErrorStatus es = acedGetCommandForDocument( this->document(), name );
	if( name != NULL )
	{
		result = LayerFilterManager::IsModalLayerCommand( name );
		acutDelString( name );
	}
	return result;
}

AcString LayerFilterDocData::ActiveLayerFilter()
{
	return LayerFilterManager::ActiveLayerFilter( this->document() );
}
#pragma once

#include "DocumentReactorTarget.h"
// #include "SelectionLayerFilter.h"
#include "AcDbObjectIdSet.h"

class LayerFilterDocData :	public CDocumentData, AcEdInputContextReactor
{
public:

	LayerFilterDocData();
	virtual ~LayerFilterDocData();
	virtual void Attach( AcApDocument * pDoc );
	virtual void Detach();

	virtual void OnPickfirstModified();

	bool filterSelectedLayer( AcDbLayerTableRecord* layer );
	bool filterRecentLayer( AcDbLayerTableRecord* layer );
	bool filterNestedLayer( AcDbLayerTableRecord* layer );

	virtual void beginQuiescentState();
	virtual void OnModelessOperationStarting( const TCHAR* context );
	virtual void OnModelessOperationEnded( const TCHAR* context );

	bool IsModalLayerDialogActive();

	void BeginModal( resbuf* rb );
	void EndModal();

	AcString ActiveLayerFilter();

private:

	void AddLayerFilters();
	int CollectSelectedLayerIds( ads_name& ss );
	int CollectNestedLayerIds();
	void UpdateLayerPropertiesManager();
	void UpdateThunk( AcDbDatabase* pDb );
	void UpdatePickfirstLayers( bool UpdateUi = false );
	void UpdateNestedPickfirstLayers( bool UpdateUi = false );
	void CollectNestedBlockRefLayers( AcDbBlockReference* pBlockRef, AcDbObjectIdSet& visited );
	void CollectBlockLayers( AcDbObjectId& blockId, AcDbObjectIdSet& blocks );
	AcDbObjectId GetBlockTableRecord( AcDbBlockReference* blockref );
	bool UpdateNeeded();

	AcDbObjectIdSet m_layers;
	AcDbObjectIdSet m_nestedLayers;
	AcDbObjectIdSet m_blocks;
	AcDbObjectIdArray m_recent;
	AcDbObjectId m_clayer;

	bool m_bUpdating;
	bool m_bDirty;
	bool m_bNestedDirty;
	bool m_bIsModal;

	bool m_bEnabled;

	//typedef CMap<AcDbObjectId, AcDbObjectIdSet> BlockLayerMap;
	//BlockLayerMap m_blockMap;

	// LayerFilterArray m_blockFilters;

};

## SelectionLayerFilter

Tony Tanzillo / ActivistInvestor

Released under the terms of the MIT License

### Background:

SelectionLayerFilter is a mixed-mode managed/native ObjectARX extension for AutoCAD that implements custom layer filters for various purposes, the main one being the ability to constrain the list of visible layers in the modal and modeless layer manager UIs to only those layers that are referenced by entities in the current/pickfirst selection. The extension works with the modal layer dialog (CLASSICLAYER), using the included QLAYER custom command.

This project was developed back in the AutoCAD 2008 timeframe, and initially targeted AutoCAD 2009 and 2010. It was never released as a public extension or product, but is being released now as a sample for educational purposes, and for those who wish to pursue its modernization and integration into current AutoCAD product releases. The project has not been modernized or upgraded to target current Visual Studio/AutoCAD versions, and is provided as-is. Anyone wishing to use this extension will need to upgrade and modernize it for their own purposes.

<center><img src="SelectedLayerFilter.gif" width="auto" height="493"></center><br>

## Custom Layer Filters:

1. The **Current Selection Layers** filter is a specialization of AcLyLayerFilter that constrains the list of visible layers to only those referenced by entitites in the current/pickfirst selection.  It is not saved with the drawing.

2. The **Nested Selection Layers** filter constrains the visibile layers to only those referenced by nested entities in the current/pickfirst selection. This filter is a child of the Current Selection Layers filter. It is not saved with the drawing.

3. The **Recently Used Layers** filter constraints the list of visible layers to only Layers that were recently created, current, or have been assigned to the layer property of one or more entities. The user can configure the number of layers to include in this filter. It is not saved with the drawing.

## The QLAYER command

The QLAYER (or 'Quick Layer') command is a custom command that is implemented by this extension. It prompts the user to select one or more entities (or uses the currently-selected entities), and then displays the modal (CLASSICLAYER) dialog with the Current Selection Layers filter active.

QLAYER is registered with ACRX_CMD_USEPICKSET.

## Development Notes

### Observing/reacting to changes to the current/pickfirst selection

In order for the Current Selection Layers filter to work with the modeless layer manager palette, its layer list must be invalidated and updated whenever the current/pickfirst selection changes. An editor reactor notification handler was implemented in form of the ```LayerFilterManager::pickfirstModified()``` method, which is called whenever the selection changes. That method in-turn calls the ```LayerFilterDocData::OnPickfirstModified()``` method, which invalidates the layer list and forces it to update. 

### Updating the modeless layer manager palette's layer list view

A very simple, non-invasive solution to the problem of coercing the layer manager palette to update its layer list is to simply assign the CLAYER system variable to its current value. Because the layer manager palette watches for changes to that system variable and updates its UI when a change is detected, that serves as a relatively simple solution.

### Restricting use of the filters to the modal and modeless layer manager palette/dialog

A design goal from the outset was to restrict the use of these custom layer filters to only be used in the Layer Manager Pallet/Dialog (both modal and modeless), but not be applied to other drop-down controls such as those on toolbars or ribbons, as their principal use is to select layers to be assigned to one or more entities. IOW, the filter should behave as if the "Apply layer filter to layer toolbar" toggle in the Layer Settings dialog was not checked. The code jumps through a few hoops to try to detect when it is being called to filter items in a drop-down control verses the layer list view in the layer manager palette/dialog, bordering on the absurd, such as managed call stack introspection. While those workarounds worked  in the product releases originally targeted by this code, they may not work in more recent product releases. Ideally, the "Apply layer filter to layer toolbar" toggle in the Layer Settings dialog should have been a per-filter setting rather than a global setting.

### Limiting the maximum number of selected objects (not implemented yet)
   To avoid significant memory/performance overhead, the selection layers filter could limit the number of selected objects it will process. This could be the GRIPOBJLIMIT or RIBBONCONTEXTSELLIM system variables, or some other costraint.





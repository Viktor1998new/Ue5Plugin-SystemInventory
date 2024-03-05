[![Epic game](https://img.shields.io/badge/epicgames-%23313131.svg?style=for-the-badge&logo=epicgames&logoColor=white)](https://www.unrealengine.com/marketplace/en-US/product/a91aca6f2ceb4237988335e01112d30e "Marketplace")
[![Discord](https://img.shields.io/badge/Discord-%235865F2.svg?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/dKNvjg2Tgg "Discord")
[![PayPal](https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/paypalme/Viktor238854 "PayPall")

#### Init
D:\NameProject\Plugins
![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/24.png)

ue4.27
``` Console
git clone --branch=ue4.27 https://github.com/Viktor1998new/Ue-Plugin-SystemInventory.git Inventory
```

ue5.0
``` Console
git clone --branch=ue5.0 https://github.com/Viktor1998new/Ue-Plugin-SystemInventory.git Inventory
```

ue5.1
``` Console
git clone --branch=ue5.1 https://github.com/Viktor1998new/Ue-Plugin-SystemInventory.git Inventory
```

ue5.2
``` Console
git clone --branch=ue5.2 https://github.com/Viktor1998new/Ue-Plugin-SystemInventory.git Inventory
```

ue5.3
``` Console
git clone --branch=ue5.3 https://github.com/Viktor1998new/Ue-Plugin-SystemInventory.git Inventory
```
********************
* Setting Plugin

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/23.jpg)

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/22.jpg)

[Watch the video](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/assets/62036206/641712bb-e713-40ad-850d-a80ec54182d9)

* Editor inventory
#### Drag and drop item: LMB
#### Menu item: RMB
#### Rotate item: Drag + Left Shift
[Watch the video](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/assets/62036206/eef027cd-7de8-40ee-95b5-cf65071eb1fc)

********************
#### ItemActor class:
The actor's logic includes two functions: "initData", which returns a read string and uses the "DataItem" function to obtain an associative array Map[Key (String), Value(String)], and "getData" function, which also uses the DataItem function, takes an associative array Map[Key (String), Value(String)] as input and returns data in a JSON-like format as a string.

[Watch the video](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/assets/62036206/847daf69-43a8-4a49-8e0d-914c92543ed0)

#### ItemAsset Class:
* The UItemAsset class is derived from the UPrimaryDataAsset class and declared using the UCLASS(BlueprintType, Blueprintable) macros for use in material graphs and Blueprint scripts.

[Watch the video](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/assets/62036206/2509ede6-6129-46a7-80e4-bae3e0743e06)

#### This class has two properties:

* ClassItemActor - an object of type TSubclassOf<AItemActor> that contains a reference to the AItemActor class that represents this item in the game. This property is set through the Unreal Editor and is read-only.

* SlotItemData - a structure FItemData that contains item data that will be saved in the inventory slot when placing this item in the slot. This data can also be viewed and modified through the Unreal Editor. The property is read-only.

********************
#### InventoryComponent class: 
* Contains an array of items

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/27.png)

virtual functions

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/28.png)

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/6.jpg)

* GetInventory function:
The GetInventory() function returns an instance of the FInventory class, which represents an inventory. This inventory contains an array of elements of type FInventorySlot, each of which describes a separate object in the inventory.
Additionally, FInventory has two properties: Massa - the mass of all objects in the inventory, and MaxSlot - the dimensions of the inventory as a coordinate point.
This function initializes an FInventory object using a pointer to the current object (this) and passes it to the constructor of the FInventory class.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/7.jpg)

* AddSlot function: 
This function adds an item to the inventory. It takes an FInventorySlot object that represents the item being added and a FindPositionSlot flag that indicates whether to search for a free slot in the inventory for the item. If it is necessary to find a free slot, the function checks if there is any free space for the item in the inventory and if not, searches for a free position to place the item.
If the stacking flag (EInventoryFlag::Stack) is set, the number of items in the slot will always be 1. If this flag is not set and the item cannot be stacked, the number of items will also be 1.
If the mass flag (EInventoryFlag::Mass) is set, the function updates the current mass (CurrentMassa) when adding the item to the inventory, taking into account the mass of the added items.
After successfully adding the item, the function generates the OnAddItem event, which indicates that the item was successfully added to the inventory.
 
![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/8.jpg)

* AddActorItem function: 
This function adds an item to the inventory from a given ItemActor object. It calls the AddAssetItem function, which takes the item object (ItemAsset), quantity (count), and additional data (data). In this case, the quantity is set to 1 because we are adding only one instance of the item.
The AddActorItem function returns a boolean value that indicates whether the item was successfully added to the inventory. If the operation is successful, the index of the slot into which the item was added will be saved in the Index variable.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/9.jpg)

* AddAssetItem function: 
This function is intended to add an item to the inventory. It takes a pointer to an object of type "UItemAsset", the number of items to add (Count), additional data (Data), and a reference to the Index variable.
If the passed ItemAsset object is null or Count is less than or equal to zero, the function will return false.
If the "Stack" inventory flag is set and the added item can be stacked (StackItem), the function will attempt to find an existing instance of this item in the inventory. If such an instance is found, the number of items in the found slot will be increased by the value of Count, and then the function will return true.
If the "Stack" flag is not set or the item cannot be stacked, the function will create a new slot in the inventory with the passed ItemAsset object and a Count of 1. Then, the function will return the result of calling the AddSlot function with the passed NewSlot, true, and Index.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/10.jpg)

* SetSlot function: 
The "SetSlot" function interacts with the inventory in the game and allows you to change the value of a specific inventory slot to the passed value.
The function takes two parameters: "Index", which specifies the index of the slot to be changed, and "NewValue", which contains the new value for this slot.
Before making changes, the function performs checks. If the passed object is not valid (i.e. if ItemAsset is equal to nullptr, Index is out of bounds of the Items array or Count is less than or equal to zero), the function will return false.
If the new object passed in the NewValue parameter differs from the object already in this slot, the function checks whether it is possible to move the new object to this slot. To do this, it iterates through all inventory slots and checks whether they occupy the same position as the new object. If so, the function will return false.
If the EInventoryFlag::Mass flag is set, the function also calculates the current weight of the inventory. If the new object differs from the one already in the slot, the function changes the weight value by subtracting the old value and adding the new value.
After all checks, the function changes the value of the slot and calls ChangeSlot to notify other components of the change in the slot's value. At the end of the function, it returns true if all operations were successful.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/11.jpg)

* FindFreeSlot function: 
The "FindFreeSlot" function searches for a free slot in the inventory that can accommodate an object of the specified size.
The function takes two parameters: "Size", which sets the size of the object (width and height), and "ReturnPosition", which contains the position of the first free slot in case of a successful search.
Before starting the search, the function performs several checks. If Size.X is greater than MaxSlot.X or (if the EInventoryFlag::OnlyX flag is not set) Size.Y is greater than MaxSlot.Y, the function returns false. If Items.Num() is zero (i.e., the inventory is empty), the function returns true and the value of ReturnPosition will be FIntPoint::ZeroValue.
Then the function searches for a free slot by iterating through all slots in the inventory. It uses variables X and Y to store the current coordinates and two for loops to iterate through them.
If the EInventoryFlag::OnlyX flag is set, the function checks if the object of the specified size can be placed on a single line horizontally. If not, the function returns false.
If the EInventoryFlag::OnlyX flag is not set, the function checks the possibility of placing the object horizontally and vertically. If the object cannot fit horizontally, the X variable is reset to zero, Y is incremented by one, and the next row is checked. If the object cannot fit vertically as well, the function returns false.
Then the function tries to place the object in the current slot (at coordinates X and Y) using the IsPositionFree method. If the slot is free, the function writes the coordinates of the returned slot to ReturnPosition and returns true.
If the slot is occupied, the function moves to the next slot, moving right or down (if the EInventoryFlag::OnlyX flag is not set) until a free slot is found or all slots are checked.
After all checks have been completed, if a free slot has been found, the function writes its coordinates to the ReturnPosition parameter and returns true. If a free slot was not found, the function returns false.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/12.jpg)

* RemoveItem function: 
This function is designed to remove an item from the inventory at a specific index. If the index is not valid, the function will return false.
If the Stack flag is set in the inventory and the item to be removed has the ability to stack (i.e. has the StackItem property specified in its SlotItemData), the function will check if there are enough items of the item to be removed to meet the Count parameter. If yes, then the removal will be executed. If, after removal, the remaining quantity of items is 0, the corresponding slot in the inventory will be changed to empty, the OnRemoveItem.Broadcast() event will be called, and the element in the Items array at that index will be removed.
If the item cannot be stacked or the number of items to be removed exceeds the current number of items in the slot, the element in the Items array at that index will be removed regardless of the number of items to be removed, and the OnRemoveItem.Broadcast() event will be called. Additionally, if the Mass flag is set in the inventory, the current mass of the inventory will be calculated.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/13.jpg)

* SendItem function:
This "SendItem" function is intended for transferring an item from one inventory to another.
The "Index" argument specifies the index of the slot that contains the item to be transferred in the current inventory.
The "ToInventory" argument points to the target inventory to which the item will be transferred.
The "Count" argument indicates the number of items being transferred.
The "FindSlot" argument determines whether to find a free slot in the target inventory to place the item or use the specified coordinates.
The "Position" argument contains the coordinates of the slot in case "FindSlot" is set to false.
If the target inventory or the specified index is not valid, the function will return false.
If the "FindSlot" parameter is set to true, the function will find a suitable free slot in the target inventory and place the item there. Otherwise, the function will check if the specified location is free and if an item can be placed there.
After transferring the item from the current inventory to the target inventory, the function will remove the item from the source slot and add it to a new slot in the target inventory.
If the transfer was successful, the function will return true; otherwise, it will return false.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/14.jpg)

* IsPositionFree function: 
The "IsPositionFree" function checks if the specified area in the inventory is free to place items. The input parameters of the function are the coordinates of the top-left corner of the area (Position), its size (Size), and a variable (Index) that will contain the index of the element (if found) or the value INDEX_NONE (if not found).

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/15.jpg)

* DropItem function: 
This is a function that allows a user to drop an item from their inventory. The function takes several parameters: IndexItem, ToIndex, Count, ToPosition, Change, and FindPosition.
If the IndexItem parameter is not a valid index of the Items array, the function returns false. If the ToIndex parameter is not equal to INDEX_NONE, the function moves the item at the IndexItem position to the ToIndex position. If the HasInventoryFlag(EInventoryFlag::Stack) flag is set, the function stacks items with the same ItemAsset if they are moved to the same position. Otherwise, the function checks if the item count is greater than zero and less than or equal to the number of items in the inventory. If the item count is outside this range, the function returns false.
If the ToIndex parameter is equal to INDEX_NONE, the function places the item at the IndexItem position into the specified ToPosition. If the position is already occupied by another item, the function returns false. If the HasInventoryFlag(EInventoryFlag::LimitY) flag is set and the ToPosition would extend the inventory beyond the maximum Y value, the function returns false.
The Change and FindPosition parameters control how the function handles moving items. If Change is true, the function attempts to move the item to the specified location. If it cannot be moved, it returns false. If FindPosition is also true, the function searches for a free slot to move the item to. If no free slot exists, the function returns false.
The function returns true if the item was successfully dropped or moved, and false otherwise.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/16.jpg)

* SetRotateSlot function: 
The SetRotateSlot function is used to change the "IsRotate" property for the inventory item at the specified index. If the new value of the property is the same as the old one or if this method is called on the client, the function does nothing and simply exits.
The function saves the current position of the item before changing the value of the "IsRotate" property. Then it sets the new value and checks if the position where the item needs to be moved is free. If the position is already occupied by another item, the function reverts the "IsRotate" property to its previous state and exits.
If the new position is free, the function saves it in the item's "PositionSlot" property and calls the "ChangeSlot" method to update information about the item in the inventory.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/17.jpg)

* RecalculationMass function (editor only): 
This function is designed to recalculate the mass of an inventory. If the EInventoryFlag::Mass flag is not set for the inventory component, the function simply returns control. However, if the flag is set, the function resets the current mass of the inventory (CurrentMassa) and iterates through all items in the inventory (stored in the Items variable). For each item, if its ItemAsset exists, the function adds to the current mass a value calculated as the product of the item's mass and the number of that item (Element.ItemAsset->SlotItemData.MassItem * Element.Count). After completing the loop, the function recalculates the total mass of the inventory and stores it in the CurrentMassa variable.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/18.jpg)

********************
* InventoryGrid class: 
The UInventoryGrid class is a user interface widget designed to display a player's inventory. It inherits from UPanelWidget and contains an array of inventory slots of type UInventoryGridSlot, as well as a reference to an inventory component of type UInventoryComponent.

The class has the following properties:

ItemSlot: a reference to the inventory slot template used to create new slots
NoneSlot: a reference to the empty slot template used to fill the inventory area where there are no items
SizeSlot: the size of the inventory slot
The class also contains the following methods:

SetInventory: sets the inventory component for this widget and calls the AddNoneSlot and AddSlot methods to create empty slots and slots with items, respectively. If the previous inventory component was set, it will be removed first.
SlotAsInventorySlot: casts the widget specified in the parameter to type UInventoryGridSlot and returns it.
GetInventory: returns the inventory component associated with this widget.
RebuildWidget: rebuilds the widget if necessary.
Event_NewDataSlot: a method called when new data about an inventory slot is received from the component. Depending on the operation type (addition, removal, modification, or movement of an item), it calls the corresponding methods of this class.
ReleaseSlateResources: deletes slot resources when they are released.
The class also contains protected methods for adding and removing slots, handling events, and rebuilding the widget.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/19.jpg)

********************
* Lib Functions
The method DataItem takes an associative array (TMap) with string key-value pairs as input and returns a string that represents this associative array in the format "key:value,key:value,...".

The method DataItemToMap takes a string containing data in the format "key:value,key:value,..." as input and returns an associative array (TMap) consisting of key-value pairs.

The method DataItemSetValue takes an associative array (TMap), a key, and a new value to be set for that key as input. If the key already exists in the associative array, its value will be replaced with the new value. The modified associative array is returned.

The method GetSlotSize takes an FInventorySlot object representing a slot in the inventory as input and returns its size as an FIntPoint structure.

![Image](https://github.com/Viktor1998new/Ue-Plugin-SystemInventory/blob/Experiment/Images/20.jpg)

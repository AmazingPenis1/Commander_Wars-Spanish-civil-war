var Constructor = function()
{
    // loader for stuff which needs C++ Support
    this.init = function (terrain)
    {
        terrain.setTerrainName(qsTr("Snow"));
    };
    this.getDefense = function()
    {
        return 1;
    };
    this.loadBaseSprite = function(terrain)
    {
		terrain.loadBaseSprite("snow");
    };
    this.getMiniMapIcon = function()
    {
        return "minimap_snow";
    };
    this.loadOverlaySprite = function(terrain)
    {
        var surroundingsPlains = terrain.getSurroundings("PLAINS,WASTELAND", true, false, GameEnums.Directions_Direct, false);
        if (surroundingsPlains.includes("+N"))
        {
            terrain.loadOverlaySprite("plains+N");
        }
        if (surroundingsPlains.includes("+E"))
        {
            terrain.loadOverlaySprite("plains+E");
        }
        if (surroundingsPlains.includes("+S"))
        {
            terrain.loadOverlaySprite("plains+S");
        }
        if (surroundingsPlains.includes("+W"))
        {
            terrain.loadOverlaySprite("plains+W");
        }
    };
};
Constructor.prototype = TERRAIN;
var SNOW = new Constructor();
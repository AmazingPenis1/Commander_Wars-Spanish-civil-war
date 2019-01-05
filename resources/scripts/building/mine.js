var Constructor = function()
{
    // called for loading the main sprite
    this.loadSprites = function(building)
    {
        if (building.getOwnerID() >= 0)
        {
            // none neutral player
            building.loadSprite("mine", false);
            building.loadSprite("mine+mask", true);
        }
        else
        {
            // neutral player
            building.loadSprite("mine+neutral", false);
        }
    };
}

Constructor.prototype = BUILDING;
var MINE = new Constructor();
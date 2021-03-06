var Constructor = function()
{
    this.getMaxUnitCount = function()
    {
        return 1;
    };
    this.armyData = [["os", "os"],
                     ["bm", "bm"],
                     ["ge", "ge"],
                     ["yc", "yc"],
                     ["bh", "bh"],
                     ["bg", "bh"],
                     ["ma", "ma"],];

    this.loadStandingAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_T_HELI.loadSprite(sprite, unit, defender, weapon, Qt.point(0, 0), 0);
    };

    this.loadSprite = function(sprite, unit, defender, weapon, movement, moveTime)
    {
        var player = unit.getOwner();
        // get army name
        var armyName = Global.getArmyNameFromPlayerTable(player, BATTLEANIMATION_T_HELI.armyData);
        sprite.loadMovingSpriteV2("t_heli+" + armyName + "+mask", GameEnums.Recoloring_Table,
                          BATTLEANIMATION_T_HELI.getMaxUnitCount(), Qt.point(0, 40), movement, moveTime);
        sprite.loadMovingSprite("t_heli+" + armyName,  false,
                          BATTLEANIMATION_T_HELI.getMaxUnitCount(), Qt.point(0, 40), movement, moveTime,
                          false, -1, 1.0, 0, 0, false, 50);
    }

    this.getDyingDurationMS = function()
    {
        // the time will be scaled with animation speed inside the engine
        return 1000;
    };

    this.hasDyingAnimation = function()
    {
        // return true if the unit has an implementation for loadDyingAnimation
        return true;
    };

    this.loadDyingAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_T_HELI.loadSprite(sprite, unit, defender, weapon, Qt.point(-140, -140), 600);
    };
};

Constructor.prototype = BATTLEANIMATION;
var BATTLEANIMATION_T_HELI = new Constructor();

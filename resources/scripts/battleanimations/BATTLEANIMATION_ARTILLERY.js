var Constructor = function()
{
    this.getMaxUnitCount = function()
    {
        return 5;
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
        var player = unit.getOwner();
        // get army name
        var armyName = Global.getArmyNameFromPlayerTable(player, BATTLEANIMATION_ARTILLERY.armyData);
        var offset = Qt.point(-5, 5);
        if (armyName === "ma")
        {
            offset = Qt.point(-35, 5);
        }
        sprite.loadSprite("artillery+" + armyName,  false,
                          BATTLEANIMATION_ARTILLERY.getMaxUnitCount(), offset);
        sprite.loadSpriteV2("artillery+" + armyName + "+mask", GameEnums.Recoloring_Table,
                            BATTLEANIMATION_ARTILLERY.getMaxUnitCount(), offset);
    };

    this.loadFireAnimation = function(sprite, unit, defender, weapon)
    {
        var offset = Qt.point(-5, 5);
        var player = unit.getOwner();
        // get army name
        var armyName = Global.getArmyNameFromPlayerTable(player, BATTLEANIMATION_ARTILLERY.armyData);
        sprite.loadSprite("artillery+" + armyName + "+fire",  false,
                          BATTLEANIMATION_ARTILLERY.getMaxUnitCount(), offset, 1);
        sprite.loadSpriteV2("artillery+" + armyName + "+fire+mask",  GameEnums.Recoloring_Table,
                          BATTLEANIMATION_ARTILLERY.getMaxUnitCount(), offset, 1);
        offset = Qt.point(30, 37);
        // gun
        if (armyName === "yc")
        {
            offset = Qt.point(30, 37);
        }
        else if (armyName === "ma")
        {
            offset = Qt.point(30, 57);
        }
        else if (armyName === "ge")
        {
            offset = Qt.point(30, 37);
        }
        else if (armyName === "bm")
        {
            offset = Qt.point(30, 37);
        }
        else if (armyName === "bh")
        {
            offset = Qt.point(30, 37);
        }
        sprite.loadSprite("artillery_shot",  false, sprite.getMaxUnitCount(), offset,
                          1, 1.0, 0, 500);
        sprite.loadSound("tank_shot.wav", 1, "resources/sounds/", 500);
    };

    this.getFireDurationMS = function()
    {
        // the time will be scaled with animation speed inside the engine
        return 1000;
    };
};

Constructor.prototype = BATTLEANIMATION;
var BATTLEANIMATION_ARTILLERY = new Constructor();

var Constructor = function()
{
    this.getMaxUnitCount = function()
    {
        return 1;
    };

    this.loadStandingAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_SUBMARINE.loadSprite(sprite, unit, defender, weapon, Qt.point(0, 0), 0);
    };

    this.loadSprite = function(sprite, unit, defender, weapon, movement, moveTime)
    {
        var player = unit.getOwner();
        // get army name
        var armyName = player.getArmy().toLowerCase();
        if (armyName === "bg")
        {
            armyName = "bh"
        }
        if ((armyName !== "yc") &&
            (armyName !== "ge") &&
            (armyName !== "bm") &&
            (armyName !== "bh") &&
            (armyName !== "ma"))
        {
            armyName = "os";
        }
        if (armyName === "ma")
        {
            sprite.loadSprite("submarine+" + armyName,  false,
                              BATTLEANIMATION_SUBMARINE.getMaxUnitCount(), Qt.point(0, 20), movement, moveTime, false, -1);
            sprite.loadSpriteV2("submarine+" + armyName + "+mask", GameEnums.Recoloring_Table,
                              BATTLEANIMATION_SUBMARINE.getMaxUnitCount(), Qt.point(0, 20), movement, moveTime, false, -1);
        }
        else
        {
            if(unit.getHidden() === true)
            {
                sprite.loadMovingSpriteV2("submarine+hidden+" + armyName + "+mask", GameEnums.Recoloring_Table,
                                  BATTLEANIMATION_SUBMARINE.getMaxUnitCount(), Qt.point(0, 20), movement, moveTime, false, -1);
                sprite.loadMovingSprite("submarine+hidden+" + armyName,  false,
                                  BATTLEANIMATION_SUBMARINE.getMaxUnitCount(), Qt.point(0, 20), movement, moveTime, false, -1);
            }
            else
            {
                sprite.loadMovingSpriteV2("submarine+" + armyName + "+mask", GameEnums.Recoloring_Table,
                                  BATTLEANIMATION_SUBMARINE.getMaxUnitCount(), Qt.point(0, 20), movement, moveTime, false, -1);
                sprite.loadMovingSprite("submarine+" + armyName,  false,
                                  BATTLEANIMATION_SUBMARINE.getMaxUnitCount(), Qt.point(0, 20), movement, moveTime, false, -1);
            }
        }
    };

    this.loadFireAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_SUBMARINE.loadStandingAnimation(sprite, unit, defender, weapon);
        var count = sprite.getUnitCount(5);
        for (var i = 0; i < count; i++)
        {
            if (globals.isEven(i))
            {
                sprite.loadSingleMovingSprite("torpedo", false, Qt.point(70, 30),
                                              Qt.point(60, 0), 400, false,
                                              1, 1, 1, i * 150);
            }
            else
            {
                sprite.loadSingleMovingSprite("torpedo", false, Qt.point(90, 50),
                                              Qt.point(60, 0), 400, false,
                                              1, 0.5, -1, i * 150);
            }
        }
    };

    this.getFireDurationMS = function()
    {
        // the time will be scaled with animation speed inside the engine
        return 1250;
    };

    this.loadImpactAnimation = function(sprite, unit, defender, weapon)
    {
        var count = sprite.getUnitCount(5);
        for (var i = 0; i < count; i++)
        {
            if (globals.isEven(i))
            {
                sprite.loadSingleMovingSprite("torpedo", false, Qt.point(127, 30),
                                              Qt.point(-70, 0), 400, true,
                                              1, 1, 1, i * 150, true);

                sprite.loadSingleMovingSprite("unit_explosion",  false, Qt.point(45, 30),
                                              Qt.point(0, 0), 0, false,
                                              1, 1.0, 1, 300 + i * 150);
            }
            else
            {
                sprite.loadSingleMovingSprite("torpedo", false, Qt.point(127, 50),
                                              Qt.point(-60, 0), 400, true,
                                              1, 0.5, -1, i * 150);
                sprite.loadSingleMovingSprite("unit_explosion",  false, Qt.point(57, 50),
                                              Qt.point(0, 0), 0, false,
                                              1, 0.5, -1, 300 + i * 150, true);
            }
            sprite.loadSound("impact_explosion.wav", 1, "resources/sounds/", i * 100);
        }
    };

    this.getImpactDurationMS = function()
    {
        // should be a second or longer.
        // the time will be scaled with animation speed inside the engine
        return 1500;
    };


    this.getDyingDurationMS = function()
    {
        // the time will be scaled with animation speed inside the engine
        return 1200;
    };

    this.hasDyingAnimation = function()
    {
        // return true if the unit has an implementation for loadDyingAnimation
        return true;
    };

    this.loadDyingAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_SUBMARINE.loadSprite(sprite, unit, defender, weapon, Qt.point(-140, 0), 1000);
    };
};

Constructor.prototype = BATTLEANIMATION;
var BATTLEANIMATION_SUBMARINE = new Constructor();

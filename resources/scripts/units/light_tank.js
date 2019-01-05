var Constructor = function()
{
    this.init = function(unit)
    {
        unit.ammo1 = 6;
        unit.maxAmmo1 = 6;
        unit.ammo2 = 10;
        unit.maxAmmo2 = 10;
        unit.fuel = 70;
        unit.maxFuel = 70;
        unit.baseMovementPoints = 6;
    };
    // called for loading the main sprite
    this.loadSprites = function(unit)
    {
        // load sprites
        unit.loadSprite("light_tank", false);
        unit.loadSprite("light_tank+mask", true);
    };
    this.getMovementType = function()
    {
        return "MOVE_TANK";
    };
    this.doWalkingAnimation = function(action)
    {
        var unit = action.getTargetUnit();
        var animation = GameAnimationFactory.createWalkingAnimation(unit, action);
        animation.loadSprite("light_tank+walk+mask", true, 1);
        animation.loadSprite("light_tank+walk", true, 1);
    };
}

Constructor.prototype = UNIT;
var LIGHT_TANK = new Constructor();
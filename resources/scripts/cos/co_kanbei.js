var Constructor = function()
{
    this.getCOStyles = function()
    {
        // string array containing the endings of the alternate co style
        // e.g. co_adder has ["+alt"]
        return ["+alt"];
    };

    this.init = function(co)
    {
        co.setPowerStars(4);
        co.setSuperpowerStars(3);
    };

    this.loadCOMusic = function(co)
    {
        // put the co music in here.
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Power:
                audio.addMusic("resources/music/cos/power.mp3", 992, 45321);
                break;
            case GameEnums.PowerMode_Superpower:
                audio.addMusic("resources/music/cos/superpower.mp3", 1505, 49515);
                break;
            case GameEnums.PowerMode_Tagpower:
                audio.addMusic("resources/music/cos/tagpower.mp3", 14611, 65538);
                break;
            default:
                audio.addMusic("resources/music/cos/kanbei.mp3", 4783, 60483);
                break;
        }
    };

    this.activatePower = function(co)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(GameEnums.PowerMode_Power);
        dialogAnimation.queueAnimation(powerNameAnimation);

        var units = co.getOwner().getUnits();
        var animations = [];
        var counter = 0;

        units.randomize();
        for (var i = 0; i < units.size(); i++)
        {
            var unit = units.at(i);

            var animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
            if (animations.length < 5)
            {
                animation.addSprite("power2", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
                powerNameAnimation.queueAnimation(animation);
                animations.push(animation);
            }
            else
            {
                animation.addSprite("power2", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5);
                animations[counter].queueAnimation(animation);
                animations[counter] = animation;
                counter++;
                if (counter >= animations.length)
                {
                    counter = 0;
                }
            }

        }
        units.remove();

        audio.clearPlayList();
        CO_KANBEI.loadCOMusic(co);
        audio.playRandom();
    };

    this.activateSuperpower = function(co, powerMode)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(powerMode);
        powerNameAnimation.queueAnimationBefore(dialogAnimation);

        var units = co.getOwner().getUnits();
        var animations = [];
        var counter = 0;
        units.randomize();
        for (var i = 0; i < units.size(); i++)
        {
            var unit = units.at(i);

            var animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());

            if (animations.length < 5)
            {
                animation.addSprite("power12", -map.getImageSize() * 2, -map.getImageSize() * 2, 0, 1.5, globals.randInt(0, 400));
                powerNameAnimation.queueAnimation(animation);
                animations.push(animation);
            }
            else
            {
                animation.addSprite("power12", -map.getImageSize() * 2, -map.getImageSize() * 2, 0, 1.5);
                animations[counter].queueAnimation(animation);
                animations[counter] = animation;
                counter++;
                if (counter >= animations.length)
                {
                    counter = 0;
                }
            }
        }
        units.remove();

        audio.clearPlayList();
        CO_KANBEI.loadCOMusic(co);
        audio.playRandom();
    };

    this.getCOUnitRange = function(co)
    {
        return 2;
    };
    this.getCOArmy = function()
    {
        return "YC";
    };
    this.getCostModifier = function(co, id, baseCost)
    {
        return baseCost * 0.2;
    };
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                if (isDefender)
                {
                    return 140;
                }
                return 70;
            case GameEnums.PowerMode_Power:
                return 60;
            default:
                if (co.inCORange(Qt.point(atkPosX, atkPosY), attacker))
                {
                    return 40;
                }
                break;
        }
        return 20;
    };
    this.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                return 70;
            case GameEnums.PowerMode_Power:
                return 60;
            default:
                if (co.inCORange(Qt.point(defPosX, defPosY), defender))
                {
                    return 40;
                }
                break;
        }
        return 20;
    };

    // CO - Intel
    this.getBio = function()
    {
        return qsTr("The emperor of Yellow Comet. A skilled commander who has a soft spot for his daughter.");
    };
    this.getHits = function()
    {
        return qsTr("Sonja");
    };
    this.getMiss = function()
    {
        return qsTr("Computers");
    };
    this.getCODescription = function()
    {
        return qsTr("All units have high offensive and defensive capabilities, but are expensive to deploy.");
    };
    this.getLongCODescription = function()
    {
        return qsTr("\nGlobal Effect:\nUnits have 20% stronger firepower and defense and are 20% more expensive.") +
               qsTr("\nCO Zone Effect:\nUnits have 40% stronger firepower and defense.");
    };
    this.getPowerDescription = function()
    {
        return qsTr("Increases firepower and defence of all units.");
    };
    this.getPowerName = function()
    {
        return qsTr("Morale Boost");
    };
    this.getSuperPowerDescription = function()
    {
        return qsTr("Greatly strengthens offensive and defensive abilities of all units. Counterattacks are stronger.");
    };
    this.getSuperPowerName = function()
    {
        return qsTr("Samurai Spirit");
    };
    this.getPowerSentences = function()
    {
        return [qsTr("Who shall taste my wrath?"),
                qsTr("Observe the heart of a true Samurai."),
                qsTr("Your numbers mean nothing!  Nothing can stop me!"),
                qsTr("My mind is honed for battle.  That is the essence of a samurai!"),
                qsTr("True Samurai don't know the meaning of retreat."),
                qsTr("I hope you thoroughly enjoy the taste of my sword!")];
    };
    this.getVictorySentences = function()
    {
        return [qsTr("Kanbei is victorious! We shall meet again!"),
                qsTr("Raise your voices in victory!"),
                qsTr("No enemy can stand before Kanbei!")];
    };
    this.getDefeatSentences = function()
    {
        return [qsTr("Preposterous! So many of Kanbei's forces defeated in such a short time? Withdraw!"),
                qsTr("Regardless of the odds, Kanbei should never lose this many units! Withdraw!")];
    };
    this.getName = function()
    {
        return qsTr("Kanbei");
    };
}

Constructor.prototype = CO;
var CO_KANBEI = new Constructor();

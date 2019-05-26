var Constructor = function()
{
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
                audio.addMusic("resources/music/cos/bh_power.mp3");
                break;
            case GameEnums.PowerMode_Superpower:
                audio.addMusic("resources/music/cos/bh_superpower.mp3");
                break;
            default:
                audio.addMusic("resources/music/cos/graves.mp3")
                break;
        }
    };

    this.activatePower = function(co)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(GameEnums.PowerMode_Power);
        dialogAnimation.queueAnimation(powerNameAnimation);

        CO_GRAVES.gravesDamage(co, 1, 3, powerNameAnimation);
        audio.clearPlayList();
        CO_GRAVES.loadCOMusic(co);
        audio.playRandom();
    };

    this.activateSuperpower = function(co, powerMode)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(powerMode);
        dialogAnimation.queueAnimation(powerNameAnimation);

        CO_GRAVES.gravesDamage(co, 2, 4, powerNameAnimation);
        audio.clearPlayList();
        CO_GRAVES.loadCOMusic(co);
        audio.playRandom();
    };

    this.gravesDamage = function(co, value, stunLevel, animation2)
    {
        var player = co.getOwner();
        var counter = 0;
        var playerCounter = map.getPlayerCount();
        var animation = null;
        var animations = [];

        for (var i2 = 0; i2 < playerCounter; i2++)
        {
            var enemyPlayer = map.getPlayer(i2);
            if ((enemyPlayer !== player) &&
                (player.checkAlliance(enemyPlayer) === GameEnums.Alliance_Enemy))
            {

                var units = enemyPlayer.getUnits();
                units.randomize();
                for (i = 0; i < units.size(); i++)
                {
                    var unit = units.at(i);

                    animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
                    if (animations.length < 5)
                    {
                        animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
                        animation2.queueAnimation(animation);
                        animations.push(animation);
                    }
                    else
                    {
                        animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5);
                        animations[counter].queueAnimation(animation);
                        animations[counter] = animation;
                        counter++;
                        if (counter >= animations.length)
                        {
                            counter = 0;
                        }
                    }
                    animation.writeDataInt32(unit.getX());
                    animation.writeDataInt32(unit.getY());
                    animation.writeDataInt32(value);
                    animation.writeDataInt32(stunLevel);
                    animation.setEndOfAnimationCall("CO_GRAVES", "postAnimationDamage");

                }
                units.remove();
            }
        }
    };

    this.postAnimationDamage = function(postAnimation)
    {
        postAnimation.seekBuffer();
        var x = postAnimation.readDataInt32();
        var y = postAnimation.readDataInt32();
        var damage = postAnimation.readDataInt32();
        var stunLevel = postAnimation.readDataInt32();
        if (map.onMap(x, y))
        {
            var unit = map.getTerrain(x, y).getUnit();
            if (unit !== null)
            {
                var hp = unit.getHpRounded();
                if (hp <= damage)
                {
                    // set hp to very very low
                    unit.setHp(0.001);
                }
                else
                {
                    unit.setHp(hp - damage);
                }
                if (unit.getHpRounded() <= stunLevel)
                {
                    unit.setHasMoved(true);
                }
            }

        }
    };

    this.startOfTurn = function(co)
    {
        audio.addMusic("resources/music/cos/graves.mp3")
    };

    this.getCOUnitRange = function(co)
    {
        return 3;
    };
    this.getCOArmy = function()
    {
        return "DM";
    };

    this.postBattleActions = function(co, attacker, atkDamage, defender, gotAttacked)
    {
        if (gotAttacked === false)
        {
            var stunLevel = 0;
            switch (co.getPowerMode())
            {
                case GameEnums.PowerMode_Superpower:
                    stunLevel = 4;
                    break;
                case GameEnums.PowerMode_Power:
                    stunLevel = 3;
                    break;
                default:
                    stunLevel = 2;
                    break;
            }
            if (defender.getHpRounded() <= stunLevel)
            {
                defender.setHasMoved(true);
            }
        }
    };

    // CO - Intel
    this.getBio = function()
    {
        return qsTr("A former assassin dissatisfied with where Wars World is headed. Secretly aids Hawke's cause and overtly aids Dark Matter. No one knows where his true loyalties lie.");
    };
    this.getHits = function()
    {
        return qsTr("Mystery Novels");
    };
    this.getMiss = function()
    {
        return qsTr("Romance Novels");
    };
    this.getCODescription = function()
    {
        return qsTr("Enemy units reduced to two or less HP by Graves' units become paralyzed.");
    };
    this.getPowerDescription = function()
    {
        return qsTr("Enemy units suffer one HP of damage. Enemy units with three or less HP become paralyzed.");
    };
    this.getPowerName = function()
    {
        return qsTr("Plague");
    };
    this.getSuperPowerDescription = function()
    {
        return qsTr("Enemy units suffer two HP of damage. Enemy units with four or less HP become paralyzed.");
    };
    this.getSuperPowerName = function()
    {
        return qsTr("Perdition");
    };
    this.getPowerSentences = function()
    {
        return [qsTr("Fear is a valuble tool. I suggest you learn how to use it."),
                qsTr("Do you desire death that greatly?"),
                qsTr("You must give everything if you want to win."),
                qsTr("You are ill prepared to face me."),
                qsTr("A valiant effort. But futile, nonetheless."),
                qsTr("Prepare yourself.")];
    };
    this.getVictorySentences = function()
    {
        return [qsTr("That was it? ...I overestimated you."),
                qsTr("Such a victory was... so rudely forced"),
                qsTr("Lives could have been spared had you just accepted your fate.")];
    };
    this.getDefeatSentences = function()
    {
        return [qsTr("Not planed but still not the end."),
                qsTr("It seems that i underestimate your strenght.")];
    };
    this.getName = function()
    {
        return qsTr("Graves");
    };
}

Constructor.prototype = CO;
var CO_GRAVES = new Constructor();
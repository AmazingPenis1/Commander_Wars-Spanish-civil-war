var Constructor = function()
{
    // loader for stuff which needs C++ Support
    this.init = function (terrain)
    {
        terrain.terrainName = qsTr("River");
    };
	this.loadBaseTerrain = function(terrain)
    {
		terrain.loadBaseTerrain("PLAINS");
    };
    this.loadBaseSprite = function(terrain)
    {
		var surroundings = terrain.getSurroundings("RIVER,BRIDGE,SEA", false, false, TERRAIN.Direct);
        var surroundingsSea = terrain.getSurroundings("SEA", true, false, TERRAIN.All);
        //
        if (!(surroundingsSea.includes("+N+NE") && surroundingsSea.includes("+NW")))
        {
            var surroundingsN = terrain.getSurroundings("SEA", true, false, TERRAIN.North);
            if (surroundingsN === "+N")
            {
                surroundings = surroundings.replace("+N", "");
            }
        }

        if (!(surroundingsSea.includes("SE+S+SW")))
        {
            var surroundingsS = terrain.getSurroundings("SEA", true, false, TERRAIN.South);
            if (surroundingsS === "+S")
            {
                surroundings = surroundings.replace("+S", "");
            }
        }

        if ((surroundingsSea.includes("+E") ))
        {
            if (!(surroundingsSea.includes("NE+E+SE")))
            {
                surroundings = surroundings.replace("+E", "");
            }
        }
        if ((surroundingsSea.includes("+W") && !(surroundingsSea.includes("SW+W+NW"))))
        {
            surroundings = surroundings.replace("+W", "");
        }
        terrain.loadBaseSprite("river" + surroundings);
    };
};
Constructor.prototype = TERRAIN;
var RIVER = new Constructor();
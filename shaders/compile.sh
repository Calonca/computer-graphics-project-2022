LOC="/usr/local/bin/glslc"
LOC="glslc"

$LOC PhongShader.vert -o PhongVert.spv
$LOC PhongShader.frag -o PhongFrag.spv
$LOC SkyBoxShader.vert -o SkyBoxVert.spv
$LOC SkyBoxShader.frag -o SkyBoxFrag.spv
$LOC TextShader.vert -o TextVert.spv
$LOC TextShader.frag -o TextFrag.spv
$LOC Terrain.frag -o TerrainFrag.spv
$LOC Terrain.vert -o TerrainVert.spv

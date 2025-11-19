(globalThis.TURBOPACK || (globalThis.TURBOPACK = [])).push([typeof document === "object" ? document.currentScript : undefined,
"[project]/src/game/world/constants.ts [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "BlockColors",
    ()=>BlockColors,
    "BlockType",
    ()=>BlockType,
    "CHUNK_HEIGHT",
    ()=>CHUNK_HEIGHT,
    "CHUNK_SIZE",
    ()=>CHUNK_SIZE,
    "WORLD_SIZE",
    ()=>WORLD_SIZE
]);
const CHUNK_SIZE = 16;
const CHUNK_HEIGHT = 32;
const WORLD_SIZE = 4; // Radius of chunks to generate around player
var BlockType = /*#__PURE__*/ function(BlockType) {
    BlockType[BlockType["AIR"] = 0] = "AIR";
    BlockType[BlockType["GRASS"] = 1] = "GRASS";
    BlockType[BlockType["DIRT"] = 2] = "DIRT";
    BlockType[BlockType["STONE"] = 3] = "STONE";
    BlockType[BlockType["WATER"] = 4] = "WATER";
    BlockType[BlockType["WOOD"] = 5] = "WOOD";
    BlockType[BlockType["LEAVES"] = 6] = "LEAVES";
    return BlockType;
}({});
const BlockColors = {
    [0]: 0x000000,
    [1]: 0x567d46,
    [2]: 0x795548,
    [3]: 0x808080,
    [4]: 0x40a4df,
    [5]: 0x5d4037,
    [6]: 0x2e7d32
};
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/game/world/Chunk.ts [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "Chunk",
    ()=>Chunk
]);
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/three/build/three.core.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/world/constants.ts [app-client] (ecmascript)");
;
;
class Chunk {
    chunkX;
    chunkZ;
    data;
    mesh = null;
    world;
    isDirty = true;
    constructor(x, z, world){
        this.chunkX = x;
        this.chunkZ = z;
        this.world = world;
        this.data = new Uint8Array(__TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_HEIGHT"] * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
    }
    getBlock(x, y, z) {
        if (x < 0 || x >= __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] || y < 0 || y >= __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_HEIGHT"] || z < 0 || z >= __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]) {
            return __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR;
        }
        return this.data[x + z * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + y * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]];
    }
    setBlock(x, y, z, type) {
        if (x >= 0 && x < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] && y >= 0 && y < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_HEIGHT"] && z >= 0 && z < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]) {
            this.data[x + z * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + y * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]] = type;
            this.isDirty = true;
        }
    }
    generate() {
        for(let x = 0; x < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]; x++){
            for(let z = 0; z < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]; z++){
                const wx = this.chunkX * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + x;
                const wz = this.chunkZ * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + z;
                // Simplex noise for height
                const scale = 0.03;
                const noiseVal = this.world.noise2D(wx * scale, wz * scale);
                // Map -1..1 to height roughly 4..12
                const h = Math.floor((noiseVal + 1) * 5) + 5;
                for(let y = 0; y < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_HEIGHT"]; y++){
                    let type = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR;
                    if (y < h - 1) {
                        // Caves? Maybe later. For now solid.
                        type = y < h - 4 ? __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].STONE : __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].DIRT;
                    } else if (y === h - 1) type = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].GRASS;
                    else if (y < 4) type = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].WATER; // Water level at 4
                    if (type !== __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR) {
                        this.setBlock(x, y, z, type);
                    }
                }
            }
        }
        this.isDirty = true;
    }
    updateMesh() {
        if (!this.isDirty) return;
        const positions = [];
        const normals = [];
        const colors = [];
        const indices = [];
        let indexOffset = 0;
        for(let y = 0; y < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_HEIGHT"]; y++){
            for(let z = 0; z < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]; z++){
                for(let x = 0; x < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]; x++){
                    const type = this.getBlock(x, y, z);
                    if (type === __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR) continue;
                    const colorHex = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockColors"][type];
                    const color = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Color"](colorHex);
                    // Check 6 neighbors
                    // If neighbor is AIR (or out of bounds and assuming edge is air for simplicity, 
                    // or querying world for neighbor chunk), add face.
                    // For simplicity in this demo, we only check within chunk boundaries or assume AIR at edges
                    // unless we implement cross-chunk meshing.
                    // To make it robust, we should ask World for block at (worldX, y, worldZ).
                    const wx = this.chunkX * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + x;
                    const wz = this.chunkZ * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + z;
                    const checkNeighbor = (nx, ny, nz)=>{
                        // Start with local check for speed
                        if (nx >= 0 && nx < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] && ny >= 0 && ny < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_HEIGHT"] && nz >= 0 && nz < __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]) {
                            return this.getBlock(nx, ny, nz) === __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR; // Transparent/Air
                        }
                        // Global check (slower but accurate for edges)
                        const nType = this.world.getBlock(this.chunkX * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + nx, ny, this.chunkZ * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] + nz);
                        return nType === __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR;
                    };
                    // Top
                    if (checkNeighbor(x, y + 1, z)) {
                        positions.push(x, y + 1, z + 1, x + 1, y + 1, z + 1, x + 1, y + 1, z, x, y + 1, z);
                        normals.push(0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0);
                        colors.push(color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b);
                        indices.push(indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3);
                        indexOffset += 4;
                    }
                    // Bottom
                    if (checkNeighbor(x, y - 1, z)) {
                        positions.push(x, y, z, x + 1, y, z, x + 1, y, z + 1, x, y, z + 1);
                        normals.push(0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0);
                        colors.push(color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b);
                        indices.push(indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3);
                        indexOffset += 4;
                    }
                    // Front (Z+)
                    if (checkNeighbor(x, y, z + 1)) {
                        positions.push(x, y, z + 1, x + 1, y, z + 1, x + 1, y + 1, z + 1, x, y + 1, z + 1);
                        normals.push(0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
                        colors.push(color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b);
                        indices.push(indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3);
                        indexOffset += 4;
                    }
                    // Back (Z-)
                    if (checkNeighbor(x, y, z - 1)) {
                        positions.push(x + 1, y, z, x, y, z, x, y + 1, z, x + 1, y + 1, z);
                        normals.push(0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1);
                        colors.push(color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b);
                        indices.push(indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3);
                        indexOffset += 4;
                    }
                    // Right (X+)
                    if (checkNeighbor(x + 1, y, z)) {
                        positions.push(x + 1, y, z + 1, x + 1, y, z, x + 1, y + 1, z, x + 1, y + 1, z + 1);
                        normals.push(1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0);
                        colors.push(color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b);
                        indices.push(indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3);
                        indexOffset += 4;
                    }
                    // Left (X-)
                    if (checkNeighbor(x - 1, y, z)) {
                        positions.push(x, y, z, x, y, z + 1, x, y + 1, z + 1, x, y + 1, z);
                        normals.push(-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0);
                        colors.push(color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b, color.r, color.g, color.b);
                        indices.push(indexOffset, indexOffset + 1, indexOffset + 2, indexOffset, indexOffset + 2, indexOffset + 3);
                        indexOffset += 4;
                    }
                }
            }
        }
        const geometry = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BufferGeometry"]();
        geometry.setAttribute('position', new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Float32BufferAttribute"](positions, 3));
        geometry.setAttribute('normal', new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Float32BufferAttribute"](normals, 3));
        geometry.setAttribute('color', new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Float32BufferAttribute"](colors, 3));
        geometry.setIndex(indices);
        if (this.mesh) {
            // Dispose old geometry
            this.mesh.geometry.dispose();
            this.mesh.geometry = geometry;
        } else {
            const material = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["MeshStandardMaterial"]({
                vertexColors: true
            });
            this.mesh = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Mesh"](geometry, material);
            this.mesh.castShadow = true;
            this.mesh.receiveShadow = true;
            this.mesh.position.set(this.chunkX * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"], 0, this.chunkZ * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
            this.world.scene.add(this.mesh);
        }
        this.isDirty = false;
    }
    dispose() {
        if (this.mesh) {
            this.mesh.geometry.dispose();
            this.mesh.material.dispose();
            this.world.scene.remove(this.mesh);
        }
    }
}
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/game/world/World.ts [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "World",
    ()=>World
]);
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$simplex$2d$noise$2f$dist$2f$esm$2f$simplex$2d$noise$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/simplex-noise/dist/esm/simplex-noise.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$Chunk$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/world/Chunk.ts [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/world/constants.ts [app-client] (ecmascript)");
;
;
;
class World {
    scene;
    chunks = new Map();
    noise2D;
    constructor(scene){
        this.scene = scene;
        this.noise2D = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$simplex$2d$noise$2f$dist$2f$esm$2f$simplex$2d$noise$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["createNoise2D"])();
    // Initial generation will happen via update
    }
    update(playerPosition) {
        const chunkX = Math.floor(playerPosition.x / __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
        const chunkZ = Math.floor(playerPosition.z / __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
        // Load chunks around player
        for(let x = -__TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["WORLD_SIZE"]; x <= __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["WORLD_SIZE"]; x++){
            for(let z = -__TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["WORLD_SIZE"]; z <= __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["WORLD_SIZE"]; z++){
                const cx = chunkX + x;
                const cz = chunkZ + z;
                const key = `${cx},${cz}`;
                if (!this.chunks.has(key)) {
                    const chunk = new __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$Chunk$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Chunk"](cx, cz, this);
                    chunk.generate();
                    chunk.updateMesh();
                    this.chunks.set(key, chunk);
                }
            }
        }
    // Simple unload logic could be added here (remove chunks far away)
    }
    getBlock(x, y, z) {
        const cx = Math.floor(x / __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
        const cz = Math.floor(z / __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
        const key = `${cx},${cz}`;
        const chunk = this.chunks.get(key);
        if (!chunk) return __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR; // Assume air if chunk not loaded
        const lx = x - cx * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"];
        const lz = z - cz * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"];
        return chunk.getBlock(lx, y, lz);
    }
    setBlock(x, y, z, type) {
        const cx = Math.floor(x / __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
        const cz = Math.floor(z / __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"]);
        const key = `${cx},${cz}`;
        let chunk = this.chunks.get(key);
        if (!chunk) {
            // Optional: Create chunk if not exists? For now, ignore setting blocks in void
            return;
        }
        const lx = x - cx * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"];
        const lz = z - cz * __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"];
        chunk.setBlock(lx, y, lz, type);
        chunk.updateMesh();
        // Update neighbors if on edge
        if (lx === 0) this.updateChunkMesh(cx - 1, cz);
        if (lx === __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] - 1) this.updateChunkMesh(cx + 1, cz);
        if (lz === 0) this.updateChunkMesh(cx, cz - 1);
        if (lz === __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["CHUNK_SIZE"] - 1) this.updateChunkMesh(cx, cz + 1);
    }
    updateChunkMesh(cx, cz) {
        const key = `${cx},${cz}`;
        const chunk = this.chunks.get(key);
        if (chunk) {
            chunk.isDirty = true;
            chunk.updateMesh();
        }
    }
}
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/game/player/Player.ts [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "Player",
    ()=>Player
]);
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/three/build/three.core.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/world/constants.ts [app-client] (ecmascript)");
;
;
class Player {
    camera;
    inputManager;
    world;
    container;
    position = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector3"](0, 25, 0);
    velocity = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector3"]();
    direction = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector3"]();
    euler = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Euler"](0, 0, 0, 'YXZ');
    speed = 5;
    jumpForce = 8;
    gravity = 20;
    height = 1.7;
    radius = 0.3;
    onGround = false;
    selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].STONE;
    // Interaction raycasting
    raycaster = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Raycaster"]();
    constructor(camera, inputManager, world, container){
        this.camera = camera;
        this.inputManager = inputManager;
        this.world = world;
        this.container = container;
        // Start position
        this.position.set(0, 25, 0);
        this.camera.position.copy(this.position);
        this.camera.rotation.copy(this.euler);
        this.initEvents();
    }
    initEvents() {
        document.addEventListener('mousemove', (e)=>{
            if (this.inputManager.locked) {
                this.euler.y -= e.movementX * 0.002;
                this.euler.x -= e.movementY * 0.002;
                this.euler.x = Math.max(-Math.PI / 2, Math.min(Math.PI / 2, this.euler.x));
                this.camera.quaternion.setFromEuler(this.euler);
            }
        });
        document.addEventListener('mousedown', (e)=>{
            if (this.inputManager.locked) {
                if (e.button === 0) {
                    this.removeBlock();
                } else if (e.button === 2) {
                    this.placeBlock();
                }
            } else {
                if (e.button === 0) {
                    this.container.requestPointerLock();
                }
            }
        });
        // Block selection
        window.addEventListener('keydown', (e)=>{
            if (e.key === '1') this.selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].DIRT;
            if (e.key === '2') this.selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].GRASS;
            if (e.key === '3') this.selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].STONE;
            if (e.key === '4') this.selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].WOOD;
            if (e.key === '5') this.selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].LEAVES;
            if (e.key === '6') this.selectedBlock = __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].WATER;
        });
    }
    update(dt) {
        // Movement
        const speed = this.inputManager.isKeyDown('ShiftLeft') ? this.speed * 0.5 : this.speed;
        const forward = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector3"](0, 0, -1).applyQuaternion(this.camera.quaternion);
        forward.y = 0;
        forward.normalize();
        const right = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector3"](1, 0, 0).applyQuaternion(this.camera.quaternion);
        right.y = 0;
        right.normalize();
        const moveDir = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector3"]();
        if (this.inputManager.isKeyDown('KeyW')) moveDir.add(forward);
        if (this.inputManager.isKeyDown('KeyS')) moveDir.sub(forward);
        if (this.inputManager.isKeyDown('KeyD')) moveDir.add(right);
        if (this.inputManager.isKeyDown('KeyA')) moveDir.sub(right);
        if (moveDir.length() > 0) moveDir.normalize();
        this.velocity.x = moveDir.x * speed;
        this.velocity.z = moveDir.z * speed;
        // Jump
        if (this.onGround && this.inputManager.isKeyDown('Space')) {
            this.velocity.y = this.jumpForce;
            this.onGround = false;
        }
        // Gravity
        this.velocity.y -= this.gravity * dt;
        // Physics Update
        this.move(dt);
        // Kill Z
        if (this.position.y < -30) {
            this.position.set(0, 30, 0);
            this.velocity.set(0, 0, 0);
        }
        this.camera.position.copy(this.position);
        // Add eye height
        this.camera.position.y += 1.6;
    }
    move(dt) {
        const steps = 8; // Sub-steps for collision accuracy
        const dtf = dt / steps;
        for(let i = 0; i < steps; i++){
            this.position.x += this.velocity.x * dtf;
            this.resolveCollision('x');
            this.position.z += this.velocity.z * dtf;
            this.resolveCollision('z');
            this.position.y += this.velocity.y * dtf;
            this.resolveCollision('y');
        }
    }
    resolveCollision(axis) {
        // Simple AABB vs Voxel collision
        // Check bounding box of player against blocks
        const minX = Math.floor(this.position.x - this.radius);
        const maxX = Math.floor(this.position.x + this.radius);
        const minY = Math.floor(this.position.y);
        const maxY = Math.floor(this.position.y + this.height);
        const minZ = Math.floor(this.position.z - this.radius);
        const maxZ = Math.floor(this.position.z + this.radius);
        for(let x = minX; x <= maxX; x++){
            for(let y = minY; y <= maxY; y++){
                for(let z = minZ; z <= maxZ; z++){
                    const block = this.world.getBlock(x, y, z);
                    if (block !== __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR) {
                        // Collision detected, resolve
                        if (axis === 'y') {
                            if (this.velocity.y < 0) {
                                // Landed
                                this.position.y = y + 1;
                                this.velocity.y = 0;
                                this.onGround = true;
                            } else {
                                // Hit head
                                this.position.y = y - this.height - 0.001;
                                this.velocity.y = 0;
                            }
                        } else if (axis === 'x') {
                            if (this.velocity.x > 0) {
                                this.position.x = x - this.radius - 0.001;
                            } else {
                                this.position.x = x + 1 + this.radius + 0.001;
                            }
                            this.velocity.x = 0;
                        } else if (axis === 'z') {
                            if (this.velocity.z > 0) {
                                this.position.z = z - this.radius - 0.001;
                            } else {
                                this.position.z = z + 1 + this.radius + 0.001;
                            }
                            this.velocity.z = 0;
                        }
                        return;
                    }
                }
            }
        }
        if (axis === 'y' && this.velocity.y < 0) {
            // Check if we are actually falling (no ground below)
            // This simple check resets onGround, but the collision loop above sets it to true if we hit something.
            // If we didn't hit anything in the loop above while moving down, we are in air.
            this.onGround = false;
        }
    }
    getInteractionTarget() {
        this.raycaster.setFromCamera(new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Vector2"](0, 0), this.camera);
        const intersects = this.raycaster.intersectObjects(this.world.scene.children, true); // true for recursive? Chunk meshes are direct children usually.
        // Filter for chunk meshes only if needed, but scene should mostly be chunks
        if (intersects.length > 0) {
            // We need to find the exact block.
            // Three.js raycast gives point and face.
            // Since we are grid aligned:
            const hit = intersects[0];
            if (!hit.face) return null;
            const p = hit.point;
            const n = hit.face.normal;
            // Determine the block coordinate inside the voxel grid.
            // "The hit point is on the surface of the mesh"
            // Move point slightly inside the block to get the removed block
            const removePos = p.clone().add(n.clone().multiplyScalar(-0.5));
            const placePos = p.clone().add(n.clone().multiplyScalar(0.5));
            const removeBlockPos = {
                x: Math.floor(removePos.x),
                y: Math.floor(removePos.y),
                z: Math.floor(removePos.z)
            };
            const placeBlockPos = {
                x: Math.floor(placePos.x),
                y: Math.floor(placePos.y),
                z: Math.floor(placePos.z)
            };
            return {
                remove: removeBlockPos,
                place: placeBlockPos,
                distance: hit.distance
            };
        }
        return null;
    }
    removeBlock() {
        const target = this.getInteractionTarget();
        if (target && target.distance < 6) {
            this.world.setBlock(target.remove.x, target.remove.y, target.remove.z, __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].AIR);
        }
    }
    placeBlock() {
        const target = this.getInteractionTarget();
        if (target && target.distance < 6) {
            // Don't place inside player
            // Simple AABB check
            const px = target.place.x + 0.5;
            const py = target.place.y + 0.5;
            const pz = target.place.z + 0.5;
            if (Math.abs(px - this.position.x) < 0.8 && Math.abs(py - (this.position.y + this.height / 2)) < 1.5 && Math.abs(pz - this.position.z) < 0.8) {
                return; // Too close
            }
            this.world.setBlock(target.place.x, target.place.y, target.place.z, this.selectedBlock);
        }
    }
}
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/game/engine/InputManager.ts [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "InputManager",
    ()=>InputManager
]);
class InputManager {
    keys = {};
    locked = false;
    constructor(){
        this.addEventListeners();
    }
    addEventListeners() {
        if ("TURBOPACK compile-time falsy", 0) //TURBOPACK unreachable
        ;
        window.addEventListener('keydown', (e)=>{
            this.keys[e.code] = true;
        });
        window.addEventListener('keyup', (e)=>{
            this.keys[e.code] = false;
        });
        document.addEventListener('pointerlockchange', ()=>{
            this.locked = !!document.pointerLockElement;
        });
    }
    isKeyDown(code) {
        return !!this.keys[code];
    }
    dispose() {
    // Cleanup if necessary
    }
}
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/game/engine/GameEngine.ts [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "GameEngine",
    ()=>GameEngine
]);
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$module$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__$3c$locals$3e$__ = __turbopack_context__.i("[project]/node_modules/three/build/three.module.js [app-client] (ecmascript) <locals>");
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/three/build/three.core.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$World$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/world/World.ts [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$player$2f$Player$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/player/Player.ts [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$engine$2f$InputManager$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/engine/InputManager.ts [app-client] (ecmascript)");
;
;
;
;
class GameEngine {
    container;
    renderer;
    scene;
    camera;
    inputManager;
    world;
    player;
    lastTime = 0;
    animationId = 0;
    constructor(container){
        this.container = container;
        // Renderer
        this.renderer = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$module$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__$3c$locals$3e$__["WebGLRenderer"]({
            antialias: false
        });
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
        this.renderer.shadowMap.enabled = true;
        this.renderer.shadowMap.type = __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["PCFSoftShadowMap"];
        container.appendChild(this.renderer.domElement);
        // Scene
        this.scene = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Scene"]();
        this.scene.background = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Color"](0x87CEEB); // Sky blue
        this.scene.fog = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Fog"](0x87CEEB, 20, 60);
        // Camera
        this.camera = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["PerspectiveCamera"](75, window.innerWidth / window.innerHeight, 0.1, 1000);
        // Lighting
        const ambientLight = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["AmbientLight"](0xffffff, 0.6);
        this.scene.add(ambientLight);
        const dirLight = new __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$three$2f$build$2f$three$2e$core$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["DirectionalLight"](0xffffff, 0.8);
        dirLight.position.set(50, 100, 50);
        dirLight.castShadow = true;
        dirLight.shadow.mapSize.width = 2048;
        dirLight.shadow.mapSize.height = 2048;
        dirLight.shadow.camera.near = 0.1;
        dirLight.shadow.camera.far = 200;
        dirLight.shadow.camera.left = -50;
        dirLight.shadow.camera.right = 50;
        dirLight.shadow.camera.top = 50;
        dirLight.shadow.camera.bottom = -50;
        this.scene.add(dirLight);
        // Input
        this.inputManager = new __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$engine$2f$InputManager$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["InputManager"]();
        // World
        this.world = new __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$World$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["World"](this.scene);
        // Player
        this.player = new __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$player$2f$Player$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["Player"](this.camera, this.inputManager, this.world, this.container);
        // Events
        window.addEventListener('resize', this.onResize);
    }
    start() {
        this.lastTime = performance.now();
        this.animate();
    }
    animate = ()=>{
        const time = performance.now();
        const delta = (time - this.lastTime) / 1000;
        this.lastTime = time;
        this.player.update(delta);
        this.world.update(this.player.position);
        this.renderer.render(this.scene, this.camera);
        this.animationId = requestAnimationFrame(this.animate);
    };
    onResize = ()=>{
        this.camera.aspect = window.innerWidth / window.innerHeight;
        this.camera.updateProjectionMatrix();
        this.renderer.setSize(window.innerWidth, window.innerHeight);
    };
    dispose() {
        cancelAnimationFrame(this.animationId);
        window.removeEventListener('resize', this.onResize);
        this.inputManager.dispose();
        this.renderer.dispose();
        if (this.container.contains(this.renderer.domElement)) {
            this.container.removeChild(this.renderer.domElement);
        }
    }
}
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/game/ui/UI.tsx [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "default",
    ()=>UI
]);
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/next/dist/compiled/react/jsx-dev-runtime.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/next/dist/compiled/react/index.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/world/constants.ts [app-client] (ecmascript)");
;
var _s = __turbopack_context__.k.signature();
;
;
function UI({ engineRef }) {
    _s();
    const [fps, setFps] = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useState"])(0);
    const [pos, setPos] = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useState"])({
        x: 0,
        y: 0,
        z: 0
    });
    const [selectedBlock, setSelectedBlock] = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useState"])(__TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].STONE);
    const requestRef = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useRef"])(0);
    const lastTimeRef = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useRef"])(performance.now());
    const framesRef = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useRef"])(0);
    (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useEffect"])({
        "UI.useEffect": ()=>{
            const update = {
                "UI.useEffect.update": ()=>{
                    if (engineRef.current) {
                        // Position
                        const p = engineRef.current.player.position;
                        setPos({
                            x: Math.round(p.x),
                            y: Math.round(p.y),
                            z: Math.round(p.z)
                        });
                        // Selected Block
                        setSelectedBlock(engineRef.current.player.selectedBlock);
                        // FPS
                        framesRef.current++;
                        const time = performance.now();
                        if (time - lastTimeRef.current >= 1000) {
                            setFps(framesRef.current);
                            framesRef.current = 0;
                            lastTimeRef.current = time;
                        }
                    }
                    requestRef.current = requestAnimationFrame(update);
                }
            }["UI.useEffect.update"];
            requestRef.current = requestAnimationFrame(update);
            return ({
                "UI.useEffect": ()=>cancelAnimationFrame(requestRef.current)
            })["UI.useEffect"];
        }
    }["UI.useEffect"], [
        engineRef
    ]);
    const blockName = (type)=>__TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"][type];
    return /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
        id: "ui-layer",
        children: [
            /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                style: {
                    display: 'flex',
                    flexDirection: 'column',
                    gap: '5px'
                },
                children: [
                    /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                        children: [
                            "FPS: ",
                            fps
                        ]
                    }, void 0, true, {
                        fileName: "[project]/src/game/ui/UI.tsx",
                        lineNumber: 50,
                        columnNumber: 9
                    }, this),
                    /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                        children: [
                            "Pos: ",
                            pos.x,
                            ", ",
                            pos.y,
                            ", ",
                            pos.z
                        ]
                    }, void 0, true, {
                        fileName: "[project]/src/game/ui/UI.tsx",
                        lineNumber: 51,
                        columnNumber: 9
                    }, this),
                    /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                        children: "Controls: WASD + Mouse, Space (Jump), Shift (Run)"
                    }, void 0, false, {
                        fileName: "[project]/src/game/ui/UI.tsx",
                        lineNumber: 52,
                        columnNumber: 9
                    }, this),
                    /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                        children: "Click to lock cursor"
                    }, void 0, false, {
                        fileName: "[project]/src/game/ui/UI.tsx",
                        lineNumber: 53,
                        columnNumber: 9
                    }, this),
                    /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                        children: "Keys 1-6 to select block"
                    }, void 0, false, {
                        fileName: "[project]/src/game/ui/UI.tsx",
                        lineNumber: 54,
                        columnNumber: 9
                    }, this)
                ]
            }, void 0, true, {
                fileName: "[project]/src/game/ui/UI.tsx",
                lineNumber: 49,
                columnNumber: 7
            }, this),
            /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                id: "crosshair"
            }, void 0, false, {
                fileName: "[project]/src/game/ui/UI.tsx",
                lineNumber: 57,
                columnNumber: 7
            }, this),
            /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                style: {
                    display: 'flex',
                    gap: '10px',
                    justifyContent: 'center'
                },
                children: [
                    [
                        __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].DIRT,
                        __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].GRASS,
                        __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].STONE,
                        __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].WOOD,
                        __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].LEAVES,
                        __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockType"].WATER
                    ].map((type)=>/*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                            style: {
                                width: '40px',
                                height: '40px',
                                backgroundColor: '#' + __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$world$2f$constants$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["BlockColors"][type].toString(16).padStart(6, '0'),
                                border: selectedBlock === type ? '4px solid white' : '2px solid black',
                                display: 'flex',
                                alignItems: 'center',
                                justifyContent: 'center',
                                fontSize: '10px',
                                textShadow: 'none',
                                color: 'white'
                            }
                        }, type, false, {
                            fileName: "[project]/src/game/ui/UI.tsx",
                            lineNumber: 61,
                            columnNumber: 13
                        }, this)),
                    /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                        style: {
                            marginLeft: '10px',
                            alignSelf: 'center'
                        },
                        children: [
                            "Current: ",
                            blockName(selectedBlock)
                        ]
                    }, void 0, true, {
                        fileName: "[project]/src/game/ui/UI.tsx",
                        lineNumber: 79,
                        columnNumber: 9
                    }, this)
                ]
            }, void 0, true, {
                fileName: "[project]/src/game/ui/UI.tsx",
                lineNumber: 59,
                columnNumber: 7
            }, this)
        ]
    }, void 0, true, {
        fileName: "[project]/src/game/ui/UI.tsx",
        lineNumber: 48,
        columnNumber: 5
    }, this);
}
_s(UI, "T9x2apRT6/2t7RqEYk8n5NYgYmw=");
_c = UI;
var _c;
__turbopack_context__.k.register(_c, "UI");
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
"[project]/src/app/page.tsx [app-client] (ecmascript)", ((__turbopack_context__) => {
"use strict";

__turbopack_context__.s([
    "default",
    ()=>Home
]);
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/next/dist/compiled/react/jsx-dev-runtime.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/node_modules/next/dist/compiled/react/index.js [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$engine$2f$GameEngine$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/engine/GameEngine.ts [app-client] (ecmascript)");
var __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$ui$2f$UI$2e$tsx__$5b$app$2d$client$5d$__$28$ecmascript$29$__ = __turbopack_context__.i("[project]/src/game/ui/UI.tsx [app-client] (ecmascript)");
;
var _s = __turbopack_context__.k.signature();
'use client';
;
;
;
function Home() {
    _s();
    const containerRef = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useRef"])(null);
    const engineRef = (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useRef"])(null);
    (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$index$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["useEffect"])({
        "Home.useEffect": ()=>{
            if (!containerRef.current) return;
            const engine = new __TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$engine$2f$GameEngine$2e$ts__$5b$app$2d$client$5d$__$28$ecmascript$29$__["GameEngine"](containerRef.current);
            engine.start();
            engineRef.current = engine;
            return ({
                "Home.useEffect": ()=>{
                    engine.dispose();
                }
            })["Home.useEffect"];
        }
    }["Home.useEffect"], []);
    return /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("main", {
        children: [
            /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])("div", {
                id: "game-container",
                ref: containerRef
            }, void 0, false, {
                fileName: "[project]/src/app/page.tsx",
                lineNumber: 25,
                columnNumber: 7
            }, this),
            /*#__PURE__*/ (0, __TURBOPACK__imported__module__$5b$project$5d2f$node_modules$2f$next$2f$dist$2f$compiled$2f$react$2f$jsx$2d$dev$2d$runtime$2e$js__$5b$app$2d$client$5d$__$28$ecmascript$29$__["jsxDEV"])(__TURBOPACK__imported__module__$5b$project$5d2f$src$2f$game$2f$ui$2f$UI$2e$tsx__$5b$app$2d$client$5d$__$28$ecmascript$29$__["default"], {
                engineRef: engineRef
            }, void 0, false, {
                fileName: "[project]/src/app/page.tsx",
                lineNumber: 26,
                columnNumber: 7
            }, this)
        ]
    }, void 0, true, {
        fileName: "[project]/src/app/page.tsx",
        lineNumber: 24,
        columnNumber: 5
    }, this);
}
_s(Home, "SMd2KUjw++Wy/dAmfBr2VA8gE10=");
_c = Home;
var _c;
__turbopack_context__.k.register(_c, "Home");
if (typeof globalThis.$RefreshHelpers$ === 'object' && globalThis.$RefreshHelpers !== null) {
    __turbopack_context__.k.registerExports(__turbopack_context__.m, globalThis.$RefreshHelpers$);
}
}),
]);

//# sourceMappingURL=src_a2474899._.js.map
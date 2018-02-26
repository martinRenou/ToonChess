let scene = new THREE.Scene();
scene.background = new THREE.Color('white');

let width = 500;
let height = 400;
let camera = new THREE.PerspectiveCamera(75, width/height, 0.1, 1000);

let renderer = new THREE.WebGLRenderer();
renderer.setSize(width, height);

let container = document.getElementById('3dview_container');
container.appendChild(renderer.domElement);

let lightDirection = new THREE.Vector3(-1.0, 1.0, -1.0);

// Load cel shading material
let celShadingMaterial = new THREE.RawShaderMaterial({
  uniforms: {
    lightDirection: { value: lightDirection }
  },
  vertexShader: document.getElementById('celShadingVertexShader').textContent,
  fragmentShader: document.getElementById('celShadingFragmentShader').textContent,
});

let objLoader = new THREE.OBJLoader();
objLoader.load(
  'assets/king.obj',
  function (object) {
    let mesh;
    object.traverse(function (child) {
      if (child instanceof THREE.Mesh) {
        mesh = child;
      }
    });

    // Add celShadingMaterial
    mesh.material = celShadingMaterial;

    scene.add(mesh);
  }
);

function animate() {
  requestAnimationFrame(animate);
  renderer.render(scene, camera);
}
animate();

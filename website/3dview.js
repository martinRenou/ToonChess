let container = document.getElementById('3dview_container');

let scene = new THREE.Scene();
scene.background = new THREE.Color('white');

let width = 800;
let height = 600;
let camera = new THREE.PerspectiveCamera(75, width/height, 0.001, 400);
camera.position.x = -6.36;
camera.position.y = 5.18;
camera.position.z = 9.83;
camera.lookAt(0.0, 0.0, 0.0);

let renderer = new THREE.WebGLRenderer();
renderer.setSize(width, height);
container.appendChild(renderer.domElement);

let controls = new THREE.TrackballControls(camera, renderer.domElement);
controls.rotateSpeed = 3.0;
controls.zoomSpeed = 1.2;
controls.noPan = true;
controls.dynamicDampingFactor = 0.3;

let lightDirection = new THREE.Vector3(-1.0, 1.0, -1.0);
let meshColor = new THREE.Vector4(1.0, 0.93, 0.70, 1.0);

// Load cel shading material
let celShadingMaterial = new THREE.RawShaderMaterial({
  uniforms: {
    lightDirection: { value: lightDirection },
    color: { value: meshColor },
  },
  vertexShader: document.getElementById('celShadingVertexShader').textContent,
  fragmentShader: document.getElementById('celShadingFragmentShader').textContent,
});

let normalShadingMaterial = new THREE.RawShaderMaterial({
  uniforms: {
    lightDirection: { value: lightDirection },
    color: { value: meshColor },
  },
  vertexShader: document.getElementById('celShadingVertexShader').textContent,
  fragmentShader: document.getElementById('normalShadingFragmentShader').textContent,
});

let mesh;
let objLoader = new THREE.OBJLoader();

// Callback function for the mesh selecting element
function loadMesh(name) {
  objLoader.load(
    'assets/' + name + '.obj',
    function (object) {
      object.traverse(function (child) {
        if (child instanceof THREE.Mesh) {
          mesh = child;
        }
      });

      // Add material according to the checkbox value
      if (document.getElementById('shading_checkbox').checked) {
        mesh.material = celShadingMaterial;
      } else {
        mesh.material = normalShadingMaterial;
      }
      mesh.rotateX(-Math.PI/2);
      mesh.translateZ(-2.5);

      scene.add(mesh);
    }
  );
}
document.getElementById('mesh_selector').onchange = function(element) {
  scene.remove(mesh);

  loadMesh(element.target.value.toLowerCase());
};

// Callback function for the shading
document.getElementById('shading_checkbox').onchange = function(element) {
  if (element.target.checked) {
    mesh.material = celShadingMaterial;
  } else {
    mesh.material = normalShadingMaterial;
  };
};

// Display king by default
loadMesh('king');

function animate() {
  requestAnimationFrame(animate);
  controls.update();
  renderer.render(scene, camera);
}
animate();

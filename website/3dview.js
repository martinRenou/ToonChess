let container = document.getElementById('3dview_container');

let scene = new THREE.Scene();
scene.background = new THREE.Color('white');

let containerBoundingBox = container.getBoundingClientRect()
let width = containerBoundingBox.width;
let height = containerBoundingBox.height;
let camera = new THREE.PerspectiveCamera(75, width/height, 0.01, 18);
camera.position.x = -6.36;
camera.position.y = 5.18;
camera.position.z = 9.83;
camera.lookAt(0.0, 0.0, 0.0);

let renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setSize(width, height);
container.appendChild(renderer.domElement);

let controls = new THREE.TrackballControls(camera, renderer.domElement);
controls.rotateSpeed = 3.0;
controls.zoomSpeed = 1.2;
controls.maxDistance = 15;
controls.noPan = true;
controls.dynamicDampingFactor = 0.3;

let lightDirection = new THREE.Vector3(-1.0, 1.0, -1.0);
let meshColor = new THREE.Vector4(1.0, 0.93, 0.70, 1.0);

// Load shaders material
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

let blackBorderMaterial = new THREE.RawShaderMaterial({
  vertexShader: document.getElementById('blackBorderVertexShader').textContent,
  fragmentShader: document.getElementById('blackBorderFragmentShader').textContent,
  side: THREE.BackSide,
});

let mesh;
let blackBorder;
let objLoader = new THREE.OBJLoader();

// Callback function for the mesh selecting element
function loadMesh(name) {
  objLoader.load(
    'assets/' + name + '.obj',
    function (object) {
      object.traverse(function (child) {
        if (child instanceof THREE.Mesh) {
          mesh = child;
          blackBorder = child.clone();
        }
      });

      // Add material according to the checkbox value
      if (document.getElementById('shading_checkbox').checked) {
        mesh.material = celShadingMaterial;
      } else {
        mesh.material = normalShadingMaterial;
      }

      // Dispay black border according to the checkbox value
      blackBorder.material = blackBorderMaterial;
      blackBorder.visible = document.getElementById('blackborder_checkbox').checked;

      blackBorder.rotateX(-Math.PI/2);
      blackBorder.translateZ(-2.5);

      mesh.rotateX(-Math.PI/2);
      mesh.translateZ(-2.5);

      scene.add(mesh);
      scene.add(blackBorder);
    }
  );
}
document.getElementById('mesh_selector').onchange = function(element) {
  scene.remove(mesh);
  scene.remove(blackBorder);

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

// Callback function for the blackborder
document.getElementById('blackborder_checkbox').onchange = function(element) {
  blackBorder.visible = document.getElementById('blackborder_checkbox').checked;
};

// Display king by default
loadMesh('king');

// Event listener for window resizing
function onWindowResize(){
  containerBoundingBox = container.getBoundingClientRect()
  width = containerBoundingBox.width;
  height = containerBoundingBox.height;
  camera.aspect = width / height;
  camera.updateProjectionMatrix();

  renderer.setSize(width, height);
}
window.addEventListener('resize', onWindowResize, false);

function animate() {
  requestAnimationFrame(animate);
  controls.update();
  renderer.render(scene, camera);
}
animate();

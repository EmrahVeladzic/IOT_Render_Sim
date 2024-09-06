import * as THREE from './node_modules/three/build/three.module.js';
import { globalModel } from './globalmodel.js';
import { waitForServerReady } from './network.js';

let done = false;


document.addEventListener('click', async ()=>{
    
    if(!done){

        done=true;

        waitForServerReady().then(()=>{


            

            const scene = new THREE.Scene();
            
            
            
            const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
            
            
            const renderer = new THREE.WebGLRenderer();
            renderer.setSize(window.innerWidth, window.innerHeight);
            document.body.appendChild(renderer.domElement);
            
           

            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            const audiodata = new Float32Array(globalModel.sound.data);
    
           
    
            for(let i = 0; i < audiodata.length; i++){
                audiodata[i] = audiodata[i] / (audiodata[i] < 0 ? 32768 : 32767);
            }
    
    
            const audioBuffer = audioContext.createBuffer(1, audiodata.length, globalModel.sound.sample_rate);
            
            audioBuffer.copyToChannel(audiodata,0);
            const source = audioContext.createBufferSource();
            source.buffer = audioBuffer;
            source.loop=true;
            
            source.connect(audioContext.destination);
            
            source.start();
            
            let clut = new Uint16Array(globalModel.image.width*globalModel.image.height);
            
            for(let i = 0; i <(globalModel.image.width*globalModel.image.height); i++ ){
            
                var tempCLR = globalModel.image.data[i];
            
                clut[i]=tempCLR;
            }
            
            const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
            directionalLight.position.set(1, 1, 1).normalize();
            scene.add(directionalLight);
            
            const tex = new THREE.DataTexture(clut, globalModel.image.width, globalModel.image.height,THREE.RGBAFormat, THREE.UnsignedShort5551Type);
            tex.needsUpdate=true;
            
            
            const loader = new THREE.FileLoader();
            let vertex_shader, fragment_shader;
            
            loader.load('vertex.glsl', function(data) {
                vertex_shader = data;
               
            });
            
            loader.load('fragment.glsl', function(data) {
                fragment_shader = data;
                
            });
            
            const loadShader = (path) => {
                return new Promise((resolve, reject) => {
                    loader.load(path, resolve, undefined, reject);
                });
            };
            

            
            let SM =[];

          
            
            Promise.all([loadShader('vertex.glsl'), loadShader('fragment.glsl')])
                .then(([vertex_shader, fragment_shader]) => {
                    globalModel.meshes.forEach(mesh => {
            
                    const j_ind = mesh.joint_index;
            
                    const geometry = new THREE.BufferGeometry();
                    const verts = new Float32Array(mesh.vertices);
                    const indices = new Uint16Array(mesh.indices);
                    const uvs = new Float32Array(mesh.uvs);
                    const norms = new Float32Array(mesh.normals);

                    
                    geometry.setAttribute('position', new THREE.BufferAttribute(verts, 3));
                    geometry.setIndex(new THREE.BufferAttribute(indices, 1));
                    geometry.setAttribute('normal', new THREE.BufferAttribute(norms, 3));
                    geometry.setAttribute('uv',new THREE.BufferAttribute(uvs, 2));
            
                    const material = new THREE.ShaderMaterial();
                    material.side=THREE.DoubleSide;
                    material.DataTexture = tex;
                    material.vertexShader = vertex_shader;
                    material.fragmentShader = fragment_shader;
            
                   
                    
                    
                    material.uniforms = {
            
                        trans_mat : {value: globalModel.joints[j_ind].matrix},
                        samp: {value: tex}
            
                        
                    };
            
            
                    const threemsh = new THREE.Mesh(geometry,material);
            
                    

                    scene.add(threemsh);
                    SM.push(threemsh);
            
                    
                }
            
            )});
            
            camera.position.x= 0;
            camera.position.z = 10;
            camera.position.y = 0;
                                  
            
            function animate() {
                requestAnimationFrame(animate);
            
             
                let currentGlobalTime = performance.now() / 1000.0; 
             
                globalModel.reset_pose();    
            
                
                globalModel.interpolate_matrix(currentGlobalTime);    
            
            
                globalModel.add_parent_matrix(new THREE.Matrix4().identity(),globalModel.root);  
               
                
                for(let i =0; i < SM.length; i++){
            
                    SM[i].material.uniforms.trans_mat.value = globalModel.joints[globalModel.meshes[i].joint_index].matrix;
            
                }
            
                
                renderer.render(scene, camera);
                scene.rotateOnAxis(new THREE.Vector3(0.0,1.0,0.0),0.001);
            
                
                
               
            
            }
            
            
            
            animate();
            
            });
    
    

    }


});


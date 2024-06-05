class AnimationBuffer{

    translations;
    rotations;
    scales;
    trans_times;
    rot_times;
    scal_times;
    joint_index;
}

class JointBuffer{

    translation;
    rotation;
    scale;
    transformMat;    
    children;

}

class MeshBuffer{

    vertices;
    indices;
    uvs;
    normals;
    joint_index;

}

class ImageBuffer{

    data;
    width;
    height;

}

class SoundBuffer{

    data;
    sample_count;
    sample_rate;
    
}

class Model{    

    joints;
    meshes;
    animations;

    root;

    image;
    sound;

}

export {Model,ImageBuffer,SoundBuffer,MeshBuffer,JointBuffer,AnimationBuffer}
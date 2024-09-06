using System.Numerics;
using System.Runtime.CompilerServices;

namespace Renderer_Backend.Models
{

    public class AnimationBuffer
    {
        public List<float>? Translations{ get; set; }
        public List<float>? Rotations{ get; set; }
        public List<float>? Scales{ get; set; }

        public List<float>? Trans_Times{ get; set; }
        public List<float>? Rot_Times{ get; set; }
        public List<float>? Scal_Times{ get; set; }

        public byte Joint_Index{ get; set; }

        public AnimationBuffer()
        {
            this.Translations = new List<float>();
            this.Rotations = new List<float>();
            this.Scales = new List<float>();
            this.Trans_Times = new List<float>();
            this.Rot_Times = new List<float>();
            this.Scal_Times = new List<float>();
        }
    }

    public class JointBuffer
    {
        
        public List<float> Translation { get; set; }
        public List<float> Rotation { get; set; }
        public List<float> Scale { get; set; }

        public bool Root{ get; set; }
        public List<byte>? Children{ get; set; }
        public List<AnimationBuffer>? Animations{ get; set; }

        public JointBuffer()
        {
            
            this.Translation = new List<float>();
            this.Rotation = new List<float>();  
            this.Scale = new List<float>();
            this.Children = new List<byte>();
            this.Animations = new List<AnimationBuffer>();
        }
    }

    public class MeshBuffer
    {
        public List<float>? Vertices{ get; set; }
        public List<UInt16>? Indices{ get; set; }
        public List<float>? Normals{ get; set; }
        public List<float>? UVs{ get; set; }

        public byte Joint_Index{ get; set; }

        public MeshBuffer()
        {
            this.Vertices = new List<float>();
            this.Indices = new List<UInt16>();
            this.Normals = new List<float>();
            this.UVs = new List<float>();
        }
    }

    public class SoundBuffer
    {
        public UInt32 Sample_Count{ get; set; }
        public UInt16 Sample_Rate{ get; set; }
        public List<Int16>? Samples{ get; set; }

        public SoundBuffer()
        {
            this.Samples = new List<Int16>();
        }
    }

    public class ImageBuffer
    {
        public UInt16 Width{ get; set; }
        public UInt16 Height{ get; set; }

        public List<UInt16>? Data{ get; set; }

        public ImageBuffer()
        {
            this.Data = new List<UInt16>();
        }
    }

    public class Model
    {
        public List<MeshBuffer>? Meshes { get; set; }
        public List<JointBuffer>? Bones{ get; set; }
        public byte Current_Anim{ get; set; }
        public byte Anim_Count { get; set; }

        public SoundBuffer? Sound{ get; set; }
        public ImageBuffer? Texture{ get; set; }

        public Model()
        {
            this.Meshes = new List<MeshBuffer>();
            this.Bones = new List<JointBuffer>();
            this.Sound = new SoundBuffer();
            this.Texture = new ImageBuffer();
        }

      

    }


}

using Renderer_Backend.Global;
using System.IO.Ports;
using System.Reflection;
using System.Text.Json;
using System.Text.Json.Nodes;
using Renderer_Backend.Models;

namespace Renderer_Backend.Serial
{
    public static class Protocol
    {
        public const string REQ_I = "R_ID\n";
        public const string REQ_L = "R_LIST\n";
        public const string REQ_R = "R_RESET\n";
        public const string REQ_N = "R_REQ\n";

        public const string R_R = "R_READY";
        public const string R_S = "R_SWITCH";
        public const string R_D = "ESP_RENDERER";

        public const int BAUD = 115200;

        public const int SUOT = 1000;
    }

    public static class Renderer
    {
        public static string? Renderer_COM {  get; set; }
        public static List<string>? Ports { get; set; }
        public static List<string>? Assets { get; set; }
        public static SerialPort? V_Port {get;set;}
        public static string? Response {  get; set; }
        public static JsonDocument? ResponseDocument { get; set; }
       
        public static bool Ready { get; set; }
        public static bool Active { get; set; }


        public static bool Initialize(string COM)
        {                                  

            V_Port = new SerialPort(COM);
            V_Port.BaudRate = Protocol.BAUD;                           
           

            V_Port.Open();

            Reset();

            Thread.Sleep(Protocol.SUOT);

            Response = V_Port.ReadExisting();

            V_Port.WriteLine(Protocol.REQ_I);

           
            Thread.Sleep(Protocol.SUOT);
            

            Response = V_Port.ReadExisting();
            ResponseDocument = JsonDocument.Parse(Response);

            
            

            if (Protocol.R_D.Equals(ResponseDocument.RootElement.GetProperty("ID").ToString()))
            {
               Active = true;
               return true;
            }

            else
            {
                V_Port.Close();
                return false;
            }

           
        }

        public static bool Initialize()
        {

            Default();

            if (V_Port != null)
            {
                V_Port.Close();
            }


            Ports = SerialPort.GetPortNames().ToList();

            foreach (string com in Ports)
            {
                if (Initialize(com))
                {
                    return true;
                }
            }

            return false;
        }

        public static void PopulateAssetList()
        {
            V_Port!.WriteLine(Protocol.REQ_L);
            Response=V_Port.ReadLine();

            ResponseDocument=JsonDocument.Parse(Response);
            
            int AssetCount = ResponseDocument.RootElement.GetProperty("Assets").GetArrayLength();

            for (int i = 0; i < AssetCount; i++)
            {
                Assets!.Add(ResponseDocument.RootElement.GetProperty("Assets")[i].ToString());
                Console.WriteLine(Assets![i]);
            }

           

           
        }


        public static void RequestData()
        {
            while (true) {

                V_Port!.WriteLine(Protocol.REQ_N);
                Response=V_Port!.ReadLine();
                ResponseDocument = JsonDocument.Parse(Response);

                if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Texture"))
                {
                    UInt16.TryParse(ResponseDocument.RootElement.GetProperty("Width").ToString(),out UInt16 W);
                    GlobalModel.Instance!.Texture!.Width = W;

                    UInt16.TryParse(ResponseDocument.RootElement.GetProperty("Height").ToString(), out UInt16 H);
                    GlobalModel.Instance!.Texture!.Height = H;

                    int d_l = ResponseDocument.RootElement.GetProperty("Data").GetArrayLength();

                    List<UInt16> d = new List<UInt16>();

                    for (int i = 0; i < d_l; i++)
                    {
                        UInt16.TryParse(ResponseDocument.RootElement.GetProperty("Data")[i].ToString(), out UInt16 DT);
                        d.Add(DT);
                    }

                    GlobalModel.Instance!.Texture!.Data = d.ToList();
                }


                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("SFX"))
                {
                    UInt16.TryParse(ResponseDocument.RootElement.GetProperty("SampleRate").ToString(), out UInt16 SR);
                    GlobalModel.Instance!.Sound!.Sample_Rate = SR;

                    UInt32.TryParse(ResponseDocument.RootElement.GetProperty("SampleCount").ToString(), out UInt32 SC);
                    GlobalModel.Instance!.Sound!.Sample_Count = SC;

                    int d_l = ResponseDocument.RootElement.GetProperty("Data").GetArrayLength();

                    List<Int16> d = new List<Int16>();

                    for (int i = 0; i < d_l; i++)
                    {
                        Int16.TryParse(ResponseDocument.RootElement.GetProperty("Data")[i].ToString(), out Int16 DT);
                        d.Add(DT);
                    }

                    GlobalModel.Instance!.Sound!.Samples = d.ToList();
                }


                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Mesh"))
                {
                    MeshBuffer msh = new MeshBuffer();

                    byte.TryParse(ResponseDocument.RootElement.GetProperty("Bone").ToString(),out byte bn);

                    msh.Joint_Index = bn;

                    int v_l = ResponseDocument.RootElement.GetProperty("Vertices").GetArrayLength();                                        

                    for (int i = 0; i < v_l; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("Vertices")[i].ToString(), out float v_f);
                        msh.Vertices!.Add(v_f);

                    }

                    int u_l = ResponseDocument.RootElement.GetProperty("UVs").GetArrayLength();

                    for (int i = 0; i < u_l; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("UVs")[i].ToString(), out float u_f);
                        msh.UVs!.Add(u_f);

                    }

                    int n_l = ResponseDocument.RootElement.GetProperty("Normals").GetArrayLength();

                    for (int i = 0; i < n_l; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("Normals")[i].ToString(), out float n_f);
                        msh.Normals!.Add(n_f);

                    }

                    int i_l = ResponseDocument.RootElement.GetProperty("Indices").GetArrayLength();

                    for (int i = 0; i < i_l; i++)
                    {
                        UInt16.TryParse(ResponseDocument.RootElement.GetProperty("Indices")[i].ToString(), out UInt16 i_i);
                        msh.Indices!.Add(i_i);

                    }


                    GlobalModel.Instance!.Meshes!.Add(msh);
                }

                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Bone"))
                {
                    JointBuffer jnt = new JointBuffer();

                    bool.TryParse(ResponseDocument.RootElement.GetProperty("Root").ToString(), out bool rt);
                    jnt.Root = rt;

                   

                    for (int i = 0; i < 3; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("T_Init")[i].ToString(), out float t);
                        jnt.Translation.Add(t);
                    
                    }

                    for (int i = 0; i < 4; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("R_Init")[i].ToString(), out float r);
                        jnt.Rotation.Add(r);
                    }

                    for (int i = 0; i < 3; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("S_Init")[i].ToString(), out float s);
                        jnt.Scale.Add(s);
                    }

                    int chd = ResponseDocument.RootElement.GetProperty("Children").GetArrayLength();

                    for (int i = 0; i < chd; i++)
                    {
                        byte.TryParse(ResponseDocument.RootElement.GetProperty("Children")[i].ToString(), out byte c);
                        jnt.Children!.Add(c);
                    }

                    GlobalModel.Instance!.Bones!.Add(jnt);

                    if (rt)
                    {
                        GlobalModel.Instance!.Root = (byte)(GlobalModel.Instance!.Bones.Count-1);
                    }

                }

                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Animation"))
                {
                    AnimationBuffer anm = new AnimationBuffer();

                    byte.TryParse(ResponseDocument.RootElement.GetProperty("Bone").ToString(),out byte bn);

                    anm.Joint_Index = bn;

                    int t = ResponseDocument.RootElement.GetProperty("T_Times").GetArrayLength();
                    int r = ResponseDocument.RootElement.GetProperty("R_Times").GetArrayLength();
                    int s = ResponseDocument.RootElement.GetProperty("S_Times").GetArrayLength();

                    for(int i = 0; i < t; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("T_Times")[i].ToString(), out float tt);
                        anm.Trans_Times!.Add(tt);
                    }

                    for (int i = 0; i < r; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("R_Times")[i].ToString(), out float rt);
                        anm.Rot_Times!.Add(rt);
                    }

                    for (int i = 0; i < s; i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("S_Times")[i].ToString(), out float st);
                        anm.Scal_Times!.Add(st);
                    }


                    for (int i = 0; i < (t * 3); i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("Translations")[i].ToString(), out float tr);
                        anm.Translations!.Add(tr);
                    }

                    for (int i = 0; i < (r * 4); i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("Rotations")[i].ToString(), out float ro);
                        anm.Rotations!.Add(ro);
                    }

                    for (int i = 0; i < (s * 3); i++)
                    {
                        float.TryParse(ResponseDocument.RootElement.GetProperty("Scales")[i].ToString(), out float sc);
                        anm.Scales!.Add(sc);
                    }


                    GlobalModel.Instance!.Bones![anm.Joint_Index]!.Animations!.Add(anm);

                }


                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Command"))
                {

                    GlobalModel.Instance!.Current_Anim = 0;
                    GlobalModel.Instance!.Anim_Count = (byte)(GlobalModel.Instance!.Bones![0]!.Animations!.Count);
                                                          

                    Ready = true;
                    break;
                }
            
            
            }

        }

        public static void RequestAsset(string item)
        {

            if (!Ready)
            {

                foreach (string ast in Assets!)
                {
                    if (ast.Equals(item))
                    {
                        V_Port!.WriteLine(item + '\n');
                        Thread.Sleep(Protocol.SUOT);
                        RequestData();
                        break;
                    }

                }


            }

        }

        public static void Default()
        {
            Ready = false;
            Active = false;
            Ports = new List<string>();
            Assets = new List<string>();            
            GlobalModel.Instance = new Models.Model();
        }

        public static void Reset()
        {
            V_Port!.WriteLine(Protocol.REQ_R);

            
        }

        public static async Task Update(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                if (Ready && V_Port !=null)
                {

                    try
                    {
                         Response = V_Port!.ReadLine();

                         ResponseDocument = JsonDocument.Parse(Response);

                        if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Command"))
                        {
                            if (ResponseDocument.RootElement.GetProperty("Value").ToString().Equals(Protocol.R_S))
                            {
                                GlobalModel.Instance!.Current_Anim=(byte)((int)(GlobalModel.Instance!.Current_Anim+1) % (int)GlobalModel.Instance!.Anim_Count);


                            }
                        }
                    }
                    
                    catch (Exception)
                    {
                        Default();
                       
                    }

                   

                }

                await Task.Delay(100, token);

            }

           
        }

    }
}

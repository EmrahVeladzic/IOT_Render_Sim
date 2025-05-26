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


            try
            {

                V_Port.Open();

            }
            catch
            {
                return false;
            }

            Reset();

            Thread.Sleep(Protocol.SUOT);

            Response = V_Port.ReadExisting();

            V_Port.WriteLine(Protocol.REQ_I);

           
            Thread.Sleep(Protocol.SUOT);
            

            Response = V_Port.ReadExisting();

            try
            {
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

            catch {

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
            Assets = new List<string>();

            ResponseDocument=JsonDocument.Parse(Response);
            
            int AssetCount = ResponseDocument.RootElement.GetProperty("Assets").GetArrayLength();

            for (int i = 0; i < AssetCount; i++)
            {
                Assets!.Add(ResponseDocument.RootElement.GetProperty("Assets")[i].ToString());
                
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
                   
                    GlobalModel.Instance!.Texture!.Width = ResponseDocument.RootElement.GetProperty("Width").GetUInt16();

                    GlobalModel.Instance!.Texture!.Height = ResponseDocument.RootElement.GetProperty("Height").GetUInt16();

                    GlobalModel.Instance!.Texture!.Data = ResponseDocument.RootElement
                    .GetProperty("Data")
                    .EnumerateArray()
                    .Select(element => element.GetUInt16())
                    .ToList();
                }


                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("SFX"))
                {
                   
                    GlobalModel.Instance!.Sound!.Sample_Rate = ResponseDocument.RootElement.GetProperty("SampleRate").GetUInt16();


                    GlobalModel.Instance!.Sound!.Sample_Count = ResponseDocument.RootElement.GetProperty("SampleCount").GetUInt32();

                  

                    GlobalModel.Instance!.Sound!.Samples = ResponseDocument.RootElement.GetProperty("Data").EnumerateArray().Select(element => element.GetInt16()).ToList();
                }


                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Mesh"))
                {
                    MeshBuffer msh = new MeshBuffer();                   

                    msh.Joint_Index = ResponseDocument.RootElement.GetProperty("Bone").GetByte();

                    msh.Vertices = ResponseDocument.RootElement.GetProperty("Vertices").EnumerateArray().Select(element => element.GetSingle()).ToList();
                    
                    msh.Indices = ResponseDocument.RootElement.GetProperty("Indices").EnumerateArray().Select(element => element.GetUInt16()).ToList();

                    msh.UVs = ResponseDocument.RootElement.GetProperty("UVs").EnumerateArray().Select(element => element.GetSingle()).ToList();

                    msh.Normals = ResponseDocument.RootElement.GetProperty("Normals").EnumerateArray().Select(element => element.GetSingle()).ToList();

                

                    GlobalModel.Instance!.Meshes!.Add(msh);
                }

                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Bone"))
                {
                    JointBuffer jnt = new JointBuffer();
               
                    jnt.Root = ResponseDocument.RootElement.GetProperty("Root").GetBoolean();

                    jnt.Translation = ResponseDocument.RootElement.GetProperty("T_Init").EnumerateArray().Select(element => element.GetSingle()).ToList();

                    jnt.Rotation = ResponseDocument.RootElement.GetProperty("R_Init").EnumerateArray().Select(element => element.GetSingle()).ToList();

                    jnt.Scale = ResponseDocument.RootElement.GetProperty("S_Init").EnumerateArray().Select(element => element.GetSingle()).ToList();

                    jnt.Children = ResponseDocument.RootElement.GetProperty("Children").EnumerateArray().Select(element => element.GetByte()).ToList(); 

                  
                    GlobalModel.Instance!.Bones!.Add(jnt);

                    if (jnt.Root)
                    {
                        GlobalModel.Instance!.Root = (byte)(GlobalModel.Instance!.Bones.Count-1);
                    }

                }

                else if (ResponseDocument.RootElement.GetProperty("Type").ToString().Equals("Animation"))
                {
                    AnimationBuffer anm = new AnimationBuffer();             

                    anm.Joint_Index = ResponseDocument.RootElement.GetProperty("Bone").GetByte();

                    anm.Trans_Times = ResponseDocument.RootElement.GetProperty("T_Times").EnumerateArray().Select(element => element.GetSingle()).ToList();
                    anm.Rot_Times = ResponseDocument.RootElement.GetProperty("R_Times").EnumerateArray().Select(element => element.GetSingle()).ToList();
                    anm.Scal_Times = ResponseDocument.RootElement.GetProperty("S_Times").EnumerateArray().Select(element => element.GetSingle()).ToList();

                    anm.Translations = ResponseDocument.RootElement.GetProperty("Translations").EnumerateArray().Select(element => element.GetSingle()).ToList();
                    anm.Rotations = ResponseDocument.RootElement.GetProperty("Rotations").EnumerateArray().Select(element => element.GetSingle()).ToList();
                    anm.Scales = ResponseDocument.RootElement.GetProperty("Scales").EnumerateArray().Select(element => element.GetSingle()).ToList();



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

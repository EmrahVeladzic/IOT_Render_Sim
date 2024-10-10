namespace Renderer_Backend.Global
{
    public class Report
    {
        public bool Ready { get; set; }
        public byte Anim {  get; set; }

        public Report(bool rdy, byte anm)
        {
            Ready = rdy;
            Anim = anm;
        }

    }
}

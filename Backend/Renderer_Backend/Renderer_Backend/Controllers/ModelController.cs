using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Renderer_Backend.Global;
using Renderer_Backend.Models;


namespace Renderer_Backend.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class ModelController : ControllerBase
    {
        [HttpGet("Model")]
        public Model GetModel()
        {
            return GlobalModel.Instance!;
        }

        [HttpGet("Anim")]
        public byte GetCurrentAnimation()
        {
            return GlobalModel.Instance!.Current_Anim;
        }

    }
}

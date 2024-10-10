using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Renderer_Backend.Global;
using Renderer_Backend.Models;
using Renderer_Backend.Serial;


namespace Renderer_Backend.Controllers
{
    [AllowAnonymous]
    [Route("api/[controller]")]
    [ApiController]
    public class ModelController : ControllerBase
    {
        [HttpGet("Model")]
        public Model GetModel()
        {
            if (Renderer.Ready)
            {
                return GlobalModel.Instance!;
            }

            else
            {
                return new Model();
            }
           
        }

        [HttpGet("Status")]
        public Report GetCurrentAnimation()
        {
            return new Report(Renderer.Ready, GlobalModel.Instance!.Current_Anim);
        }

    }
}

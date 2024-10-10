using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Renderer_Backend.Serial;

namespace Renderer_Backend.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class RendererController : ControllerBase
    {
        [HttpGet("Initialize")]
        public void Init()
        {
           if(Renderer.Initialize())
           {
               Renderer.PopulateAssetList();
           }
        }

        [HttpGet("Select")]
        public void Select(string ast)
        {
            if (Renderer.Active)
            {
                Renderer.RequestAsset(ast);
            }
        }

       
    }
}

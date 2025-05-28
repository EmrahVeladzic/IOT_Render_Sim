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
        public List<string> Init()
        {
           if(Renderer.Initialize())
           {
               Renderer.PopulateAssetList();
               
           }
            return Renderer.Assets!;
        }

        [HttpGet("Select")]
        public void Select(string ast = "DEFAULT")
        {
            if (Renderer.Active && Renderer.Assets!=null && Renderer.Assets.Count()>0)
            {
                string Asset_Select = ast.ToUpper();

                if (Renderer.Assets!.Contains(Asset_Select))
                {
                    Renderer.RequestAsset(Asset_Select);
                }

                else
                {
                    Renderer.RequestAsset(Renderer.Assets[0]);
                }

                
            }
        }

       
    }
}

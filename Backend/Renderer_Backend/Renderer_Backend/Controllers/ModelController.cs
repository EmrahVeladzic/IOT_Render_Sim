﻿using Microsoft.AspNetCore.Authorization;
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
        public IActionResult GetModel()
        {
            if (Renderer.Ready)
            {
                return StatusCode(200,GlobalModel.Instance!);
            }

            else
            {
                return StatusCode(200,new Model());
            }
           
        }

        [HttpGet("Status")]
        public IActionResult GetCurrentAnimation()
        {
            return StatusCode(200,new Report(Renderer.Ready, GlobalModel.Instance!.Current_Anim));
        }

    }
}

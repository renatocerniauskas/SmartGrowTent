package br.com.smartgrowtent.adapter.inbound.rest;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;


@RestController
@ResponseBody
@RequestMapping("/api/grow")
public class GrowController {

    @GetMapping
    public String health() {
        return "200 OK";
    }

//    @PostMapping
//    public ResponseEntity<?> cadastrarCliente(@RequestBody @Valid ClienteFormDto requestCliente){
//        //return ResponseEntity.status(HttpStatus.CREATED).body(requestCliente);
//        return ResponseEntity.status(HttpStatus.CREATED).body(processor.cadastrar(requestCliente));
//    }
}

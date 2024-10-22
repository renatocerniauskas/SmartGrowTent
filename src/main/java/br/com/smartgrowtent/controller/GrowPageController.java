package br.com.smartgrowtent.controller;

import br.com.smartgrowtent.repositories.GrowValuesRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
@Slf4j
public class GrowPageController {

    @Autowired
    GrowValuesRepository repository;

    @GetMapping("/grow")
    public String showGrow() {
        return "grow";
    }
}

package br.com.smartgrowtent.controller;



import br.com.smartgrowtent.models.GrowValues;
import br.com.smartgrowtent.repositories.GrowValuesRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@Controller
public class ChartController {

    @Autowired
    GrowValuesRepository repository;



//    @GetMapping("/chart")
//    public String getChartData(Model model) {
//        // Dados de exemplo
//        List<Object[]> chartData = Arrays.asList(
//                new Object[]{"Categoria", "Valor"},
//                new Object[]{"A", 30},
//                new Object[]{"B", 70},
//                new Object[]{"C", 50}
//        );
//
//        model.addAttribute("chartData", chartData);
//        return "chart";
//    }

    @GetMapping("/chart")
    public String getChartData2(Model model) {
        // Suponha que você tenha um serviço que retorna os dados do gráfico
        var growValues = repository.findAll();
        List<Object[]> chartData = new ArrayList<>();
        chartData.add(new Object[]{"Categoria", "Valor"});
        for(GrowValues growValue : growValues) {
            chartData.add(new Object[]{growValue.getTemperature(), growValue.getHumidity()});
        }

        model.addAttribute("chartData", chartData);
        return "chart";
    }

}

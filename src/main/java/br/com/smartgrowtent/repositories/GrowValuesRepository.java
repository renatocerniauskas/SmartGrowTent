package br.com.smartgrowtent.repositories;

import br.com.smartgrowtent.models.GrowValues;
import org.springframework.data.jpa.repository.JpaRepository;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

public interface GrowValuesRepository extends JpaRepository<GrowValues,Long> {
    List<GrowValues> findByTimestampBetween(LocalDateTime start, LocalDateTime end);
    GrowValues findTopByOrderByTimestampDesc();
}

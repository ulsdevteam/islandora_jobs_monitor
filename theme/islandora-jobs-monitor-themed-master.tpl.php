<div class="master" id="<?php print $host_record['hostname']; ?>">
  <div class="gauge_wrapper"><?php print $gauge; ?></div>
  <br style="clear:top">
  <?php unset($host_record['hostname']); ?>
  <?php foreach ($host_record as $key => $value): ?>
    <div class="div_label"><?php print str_replace("_", " ", ucwords($key)); ?></div>
    <div class="div_value"><?php print $value; ?></div>
    <br class="clearfix">
  <?php endforeach; ?>

  <div class="cpu_graph"><?php print $cpu_values; ?></div>
  <div class="memory_graph"><?php print $memory_values; ?></div>

  <?php print $control_links; ?>
</div>

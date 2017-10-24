<div class="div_label"><?php print $label; ?></div>
<?php foreach ($values as $timestamp => $value) : ?>
  <div class="value-wrapper" title="<?php print $value . "% at " . $timestamp; ?>">
      <div class="value<?php if ($value < 20) { print " value_good";} elseif ($value > 90) { print " value_bad"; } ?>" title="<?php print $value . "% at " . $timestamp; ?>" style="line-height: <?php print $value; ?>%">&nbsp;</div>
  </div>
<?php endforeach; ?>
<br class="clearfix">